// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "CachingPassManager.h"
#include "DebugInfoScanner.h"
#include "DependencyMap.h"
#include "Getters.h"
#include "StopWatch.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Bitcode/BitcodeWriterPass.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineOptimizationRemarkEmitter.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/Debug.h"
#include "llvm/Target/TargetMachine.h"

#include "../lib/CodeGen/CodeGenModule.h"
#include "clang/CodeGen/BackendUtil.h"
#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"

#define DEBUG_TYPE "zapcc-alive"

using namespace clang;
namespace llvm {

// See CachingCompiler::unloadDebugInfo.
static void findDI(Module &M) {
  // UserDINodes will stay empty here.
  // It is actually used in Unloader::eraseDebugTypes.
  SmallPtrSet<const DINode *, 1> UserDINodes;
  DebugInfoScanner DIS(&M, M.AliveDIs, UserDINodes);
  for (const Function &F : M)
    DIS.process(F);
  for (const GlobalVariable &GV : M.globals())
    DIS.process(GV);
  DICompileUnit *CU = getCU(M);
  // See DwarfCompileUnit::constructImportedEntityDIE().
  for (DIImportedEntity *IE : CU->getImportedEntities()) {
    DINode *DIN = DIS.resolve(IE->getEntity());
    // Alive subprograms and global variables had already been scanned above.
    if (!isa<DISubprogram>(DIN) && !isa<DIGlobalVariable>(DIN))
      DIS.process(IE);
  }

#if 0
  DIArray ETs = CU.getEnumTypes();
  for (unsigned i = 0, e = ETs.getNumElements(); i != e; ++i) {
    DIType ET(ETs.getElement(i));
    if (AliveTypes.count(&*ET))
      M.AliveDebugNodes.insert(ET);
  }

  DIArray RTs = CU.getRetainedTypes();
  for (unsigned i = 0, e = RTs.getNumElements(); i != e; ++i) {
    DIType RT(RTs.getElement(i));
    if (AliveTypes.count(&*RT))
      M.AliveDebugNodes.insert(RT);
  }
#endif
}

static const RecordDecl *isRecordDeclContext(const Decl *D) {
  if (const RecordDecl *DC = dyn_cast_or_null<RecordDecl>(D->getDeclContext()))
    return DC;
  return nullptr;
}

class CachingPassManager::Implementation {
public:
  Implementation(CompilerInstance *CI, DependencyMap *DM);
  ~Implementation() {
    if (M)
      M->invalidateMachineFunctionHandler = nullptr;
    AP = nullptr;
    MMI = nullptr;
  }
  void emitObj(BackendAction Action, raw_pwrite_stream *AsmOutStream);

private:
  template <typename T> bool isNoDefinition(const T *TD) {
    const T *R = cast<T>(TD->getMostRecentDecl());
    do {
      if (R->isThisDeclarationADefinition() ||
          R->template hasAttr<AliasAttr>()) {
        if (Context->LiveDecls.count(R))
          return false;
        if (R->isImplicit())
          if (const RecordDecl *RD = isRecordDeclContext(R))
            if (Context->LiveDecls.count(RD))
              return false;
      }
      R = cast_or_null<T>(R->getPreviousDecl());
    } while (R);
    return true;
  }
  void setGlobalValues(const Decl *D, bool Declaration) {
    DEBUG(llvm::errs() << "\nsetGlobalValues "
                       << (Declaration ? "Declaration" : "Definition") << ":\n";
          D->dump(););
    for (const Decl *R : D->redecls()) {
      SmallVector<GlobalValue *, 8> GVs;
      Builder->getDeclGlobalValues(R, GVs);
      // Construction VTables has internal linkage and must be defined.
      // See CodeGenVTables::GenerateConstructionVTable.
      for (GlobalValue *GV : GVs) {
        DEBUG(llvm::errs() << ">> " << GV->getName() << "\n";);
        if (Declaration && !GV->hasInternalLinkage())
          GV->setAliveDeclaration();
        else
          GV->setAliveDefinition();
      }
    }
  }
  void initPasses(BackendAction Action, raw_pwrite_stream *AsmOutStream);
  void resurrect();
  void createModuleInlineAsm();
  bool erase(const Function *F);
  CompilerInstance *CI;
  CodeGen::CodeGenModule *Builder;
  Module *M;
  ASTContext *Context;
  DependencyMap *DM;
  std::unique_ptr<CachingPassHelper> CPH;
  legacy::FunctionPassManager *PerFunctionPasses;
  legacy::PassManager *PerModulePasses;
  // We own CodeGenPasses.
  std::unique_ptr<legacy::PassManager> CodeGenPasses;
  // Convenience pointers only, no ownership implied.
  AsmPrinter *AP;
  MachineModuleInfo *MMI;
  AssumptionCacheTracker *ACT;
  MachineOptimizationRemarkEmitterPass *MORE;
};

CachingPassManager::Implementation::Implementation(CompilerInstance *CI,
                                                   DependencyMap *DM)
    : CI(CI), Builder(nullptr), M(nullptr), Context(nullptr), DM(DM),
      PerFunctionPasses(nullptr), PerModulePasses(nullptr), AP(nullptr),
      MMI(nullptr), MORE(nullptr) {
  assert(CI->getLangOpts().CachingMode);
}

namespace {
class ResurrectGlobals {
  SmallPtrSet<GlobalValue *, 32> SeenGlobals;
  SmallPtrSet<Constant *, 8> SeenConstants;

  void GlobalIsNeeded(GlobalValue *G) {
    if (!SeenGlobals.insert(G).second)
      return;
    if (G->isAliveDeclaration())
      return;
    G->setAliveDefinition();
    if (GlobalVariable *GV = dyn_cast<GlobalVariable>(G)) {
      if (GV->hasInitializer())
        MarkUsedGlobalsAsNeeded(GV->getInitializer());
    } else if (GlobalIndirectSymbol *GIS = dyn_cast<GlobalIndirectSymbol>(G)) {
      MarkUsedGlobalsAsNeeded(GIS->getIndirectSymbol());
    } else {
      Function *F = cast<Function>(G);
      for (Use &U : F->operands())
        MarkUsedGlobalsAsNeeded(cast<Constant>(U.get()));
      for (BasicBlock &BB : *F)
        for (Instruction &I : BB)
          for (Use &U : I.operands())
            if (GlobalValue *GV = dyn_cast<GlobalValue>(U))
              GlobalIsNeeded(GV);
            else if (Constant *C = dyn_cast<Constant>(U))
              MarkUsedGlobalsAsNeeded(C);
    }
  }
  void MarkUsedGlobalsAsNeeded(Constant *C) {
    if (GlobalValue *GV = dyn_cast<GlobalValue>(C))
      return GlobalIsNeeded(GV);
    for (Use &U : C->operands()) {
      Constant *Op = dyn_cast<Constant>(U);
      if (Op && SeenConstants.insert(Op).second)
        MarkUsedGlobalsAsNeeded(Op);
    }
  }

public:
  ResurrectGlobals(llvm::Module &M) {
    for (Function &F : M)
      GlobalIsNeeded(&F);
    for (GlobalVariable &GV : M.globals())
      GlobalIsNeeded(&GV);
    for (GlobalAlias &GA : M.aliases())
      GlobalIsNeeded(&GA);
    for (GlobalIFunc &GIS : M.ifuncs())
      GlobalIsNeeded(&GIS);
  }
};
}

static bool hasOutOfLineStaticDataMemberInstantiation(const VarDecl *VD) {
  for (const VarDecl *R : VD->redecls())
    if (R->isOutOfLine() && R->isStaticDataMember() &&
        R->getInstantiatedFromStaticDataMember())
      return true;
  return false;
}

void CachingPassManager::Implementation::resurrect() {
  SetVector<const Decl *> DeclsToDecide(Context->LiveDecls.begin(),
                                        Context->LiveDecls.end());
  for (unsigned I = 0; I < DeclsToDecide.size(); ++I) {
    if (const auto *CTSD =
            dyn_cast<ClassTemplateSpecializationDecl>(DeclsToDecide[I]))
      for (const Decl *D : CTSD->decls())
        DeclsToDecide.insert(D);
  }
  DeclsToDecide.remove_if([](const Decl *D) {
    return D->getFriendObjectKind() != Decl::FriendObjectKind::FOK_None ||
           D->getDeclContext()->isFunctionOrMethod();

  });
  for (const Decl *D : DeclsToDecide) {
    if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
      if (Builder->ErasedDeferredDeclsToEmit.count(FD)) {
        auto K = Builder->GlobalDecls.find(FD);
        assert(K != Builder->GlobalDecls.end());
        for (GlobalDecl GD : llvm::reverse(K->second))
          Builder->EmitGlobal(GD.getWithDecl(FD));
        Builder->ErasedDeferredDeclsToEmit.erase(FD);
      }
    }
  }
  for (const Decl *D : DeclsToDecide) {
    if (D->hasAttr<AliasAttr>() || D->hasAttr<UsedAttr>()) {
      if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D))
        if (FD->isTemplateInstantiation())
          continue;
      setGlobalValues(D, false);
    } else if (const VarDecl *VD = dyn_cast<VarDecl>(D)) {
      if (hasOutOfLineStaticDataMemberInstantiation(VD)) {
        auto TSK = VD->getTemplateSpecializationKind();
        if (TSK == TSK_ExplicitInstantiationDefinition ||
            DM->dependsOnEmittedFunction(VD))
          setGlobalValues(VD, false);
      } else if (VD->getTLSKind() == VarDecl::TLS_None) {
        setGlobalValues(D, isNoDefinition(VD));
      }
    } else if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
      if (isNoDefinition(FD))
        setGlobalValues(D, true);
      else if (FD->doesThisDeclarationHaveABody() &&
               Context->GetGVALinkageForFunction(FD) >
                   GVALinkage::GVA_DiscardableODR)
        setGlobalValues(D, false);
    } else if (const CXXRecordDecl *CXXRD = dyn_cast<CXXRecordDecl>(D)) {
      if (CXXRD->hasDefinition() && CXXRD->isDynamicClass())
        if (const CXXMethodDecl *KeyFunction =
                Context->getCurrentKeyFunction(CXXRD))
          setGlobalValues(CXXRD, isNoDefinition(KeyFunction));
    }
  }
  Builder->Release();
  // Obtain closure, required for globals such as string constants.
  ResurrectGlobals RG(*M);
  for (Function &F : *M) {
    if (F.isAliveDeclaration())
      continue;
    auto I = Builder->FunctionTypeInfos.find(&F);
    if (I != Builder->FunctionTypeInfos.end()) {
      for (Value *V : I->second)
        if (GlobalValue *GV = GlobalValue::fromValue(V))
          if (!GV->isAliveDeclaration())
            GV->setAliveDefinition();
    }
  }
}

void CachingPassManager::Implementation::createModuleInlineAsm() {
  M->setModuleInlineAsm("");
  for (const Decl *D : Context->LiveDecls)
    if (const FileScopeAsmDecl *FSAD = dyn_cast<FileScopeAsmDecl>(D))
      M->appendModuleInlineAsm(FSAD->getAsmString()->getString());
}

bool CachingPassManager::Implementation::erase(const Function *F) {
  ACT->erase(F);
  return MMI->erase(F);
}

void CachingPassManager::Implementation::initPasses(
    BackendAction Action, raw_pwrite_stream *AsmOutStream) {
  if (CPH)
    return;
  assert(M);
  CodeGenOptions &CGO = CI->getCodeGenOpts();
#ifdef NDEBUG
  CGO.VerifyModule = false;
#endif
  CPH.reset(new CachingPassHelper(
      CI->getDiagnostics(), CI->getHeaderSearchOpts(), CGO,
      *CI->getInvocation().TargetOpts, CI->getLangOpts(), M));
  bool UsesCodeGen = (Action != BackendAction::Backend_EmitNothing &&
                      Action != BackendAction::Backend_EmitBC &&
                      Action != BackendAction::Backend_EmitLL);
  llvm::TargetMachine *TM = CPH->CreateTargetMachine(UsesCodeGen);
  PerFunctionPasses = CPH->getPerFunctionPasses();
  PerModulePasses = CPH->getPerModulePasses();

  // clang/lib/CodeGen/BackendUtil.cpp, EmitAssemblyHelper::EmitAssembly
  if (UsesCodeGen && !TM)
    return;
  if (TM)
    M->setDataLayout(TM->createDataLayout());

  TargetIRAnalysis TIRA;
  if (TM)
    TIRA = TM->getTargetIRAnalysis();
  PerModulePasses->add(createTargetTransformInfoWrapperPass(TIRA));
  PerFunctionPasses->add(createTargetTransformInfoWrapperPass(TIRA));
  CPH->CreatePasses();
  if (!UsesCodeGen)
    return;
  CodeGenPasses = llvm::make_unique<legacy::PassManager>();
  CodeGenPasses->add(createTargetTransformInfoWrapperPass(TIRA));
  CPH->AddEmitPasses(*CodeGenPasses, Action, *AsmOutStream);
  MMI = static_cast<MachineModuleInfo *>(
      CodeGenPasses->findPass(&MachineModuleInfo::ID));
  assert(MMI);
  ACT = static_cast<AssumptionCacheTracker *>(
      CodeGenPasses->findPass(&AssumptionCacheTracker::ID));
  assert(ACT);
  AP = static_cast<AsmPrinter *>(
      CodeGenPasses->findPass(&AsmPrinter::ID));
  assert(AP);
  MORE = static_cast<MachineOptimizationRemarkEmitterPass *>(
      CodeGenPasses->findPass(&MachineOptimizationRemarkEmitterPass::ID));
  assert(AP);
  M->invalidateMachineFunctionHandler =
      std::bind(&CachingPassManager::Implementation::erase, this, std::placeholders::_1);
}

static void undoDwarfEHPrepare(Function &F) {
  for (BasicBlock &BB : F) {
    UnreachableInst *UI = dyn_cast<UnreachableInst>(BB.getTerminator());
    if (!UI)
      continue;
    CallInst *CI = dyn_cast_or_null<CallInst>(UI->getPrevNode());
    if (!CI)
      continue;
    ExtractValueInst *EVI =
        dyn_cast_or_null<ExtractValueInst>(CI->getPrevNode());
    if (!EVI)
      continue;
    if (CI->getCalledFunction()->getName() == "_Unwind_Resume") {
      ResumeInst::Create(EVI->getOperand(0), &BB);
      UI->eraseFromParent();
      CI->eraseFromParent();
      EVI->eraseFromParent();
    }
  }
}

void CachingPassManager::Implementation::emitObj(
    BackendAction Action, raw_pwrite_stream *AsmOutStream) {
  Builder = CI->CodeGen->getBuilder();
  M = getModule(CI);
  Context = &CI->getASTContext();
  initPasses(Action, AsmOutStream);
  Builder->EmitDeferred();
  {
    StopWatch SW("update");
    DM->update(Context, &CI->getPreprocessor());
  }
  resurrect();
  createModuleInlineAsm();
  PerFunctionPasses->doInitialization();
  for (Function &F : *M) {
    if (!F.isCooked())
      PerFunctionPasses->run(F);
  }
  PerFunctionPasses->doFinalization();
  PerModulePasses->run(*M);
  // CodeGenModule::CodeGenModule creates DebugInfo only if needed.
  bool DebugEnabled = getCU(*M);
  if (DebugEnabled)
    findDI(*M);
  // Reset Passed for emit passes and set later for all current module functions
  // which had already passed.
  SmallVector<Function *, 32> PassedFunctions;
  for (Function &F : *M) {
    if (F.isDeclaration())
      continue;
    F.resetPassed();
    PassedFunctions.push_back(&F);
  }
  if (Action == Backend_EmitObj) {
    MCObjectStreamer *Streamer =
        static_cast<MCObjectStreamer *>(AP->OutStreamer.get());
    Streamer->getAssembler().getWriter().setStream(*AsmOutStream);
    AP->setDebugEnabled(DebugEnabled);
    SmallVector<std::pair<Function *, MachineFunction *>, 32> CachedFunctions;
    for (Function &F : *M) {
      if (F.isDeclaration() || F.isAliveDeclaration() ||
          F.hasAvailableExternallyLinkage())
        continue;
      if (MachineFunction *MF = MMI->get(&F))
        CachedFunctions.push_back({&F, MF});
    }
    for (auto Pair : CachedFunctions)
      Pair.first->resetAlive();
    CodeGenPasses->run(*M, legacy::RK_Initialization);
    for (auto Pair : CachedFunctions) {
      MachineFunction &MF = *Pair.second;
      MORE->runOnMachineFunction(MF);
      AP->runOnMachineFunction(MF);
    }
    CodeGenPasses->run(*M, legacy::RK_Finalization);
    for (Function &F : *M)
      undoDwarfEHPrepare(F);
    for (auto Pair : CachedFunctions)
      Pair.first->setAliveDefinition();
  } else if (Action == Backend_EmitBC) {
    std::unique_ptr<ModulePass> BitcodeWriter(createBitcodeWriterPass(
        *AsmOutStream, CI->getCodeGenOpts().EmitLLVMUseLists));
    BitcodeWriter->runOnModule(*M);
  } else if (Action == Backend_EmitLL) {
    std::unique_ptr<ModulePass> PrintModule(createPrintModulePass(
        *AsmOutStream, "", CI->getCodeGenOpts().EmitLLVMUseLists));
    PrintModule->runOnModule(*M);
  }
  for (Function *F : PassedFunctions)
    F->setPassed();
  if (DebugEnabled)
    M->AliveDIs.clear();
}

CachingPassManager::CachingPassManager(CompilerInstance *CI, DependencyMap *DM)
    : Impl(new Implementation(CI, DM)) {}

CachingPassManager::~CachingPassManager() {}

void CachingPassManager::emitObj(BackendAction Action,
                                 raw_pwrite_stream *AsmOutStream) {
  Impl->emitObj(Action, AsmOutStream);
}

} // End llvm namespace
