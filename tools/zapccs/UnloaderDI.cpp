// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "DebugInfoScanner.h"
#include "Getters.h"
#include "Unloader.h"

#include "../lib/CodeGen/CGDebugInfo.h"
#include "../lib/IR/LLVMContextImpl.h"
#include "llvm/IR/Module.h"

using namespace llvm;
namespace clang {

void Unloader::eraseDebugTypes(CodeGen::CGDebugInfo *CGDI) {
  auto &TypeCache = CGDI->TypeCache;

  SmallPtrSet<const DINode *, 1> UserDINodes;
  SmallPtrSet<const DINode *, 1> UserVisited;
  DebugInfoScanner UserDIS(M, UserVisited, UserDINodes);
  DICompileUnit *CU = getCU(*M);
  if (!CU)
    return;
  for (auto &I : TypeCache) {
    if (DIType *DIT = cast_or_null<llvm::DIType>(I.second)) {
      QualType QT(QualType::getFromOpaquePtr(I.first));
      const Type *T = QT.getTypePtr();
      if (!T)
        continue;
      // Seed UserDINodes with known dead types.
      if (UD.Types.count(const_cast<Type *>(T)) ||
          TypesToKeepDeclaration.count(T)) {
        UserDINodes.insert(DIT);
      } else {
#if 0
        // Definition is unloaded: reset size, align and offset.
        // FIXME: get them back.
        if (TypesToKeepDeclaration.count(T))
          DIT->resetSizeAlignOffset();
#endif
      }
    }
  }
  // Collect all nodes.
  SmallVector<DINode *, 128> CurrentDINodes;
  for (DICompositeType *ET : CU->getEnumTypes())
    CurrentDINodes.push_back(ET);
  for (DIScope *RT : CU->getRetainedTypes())
    CurrentDINodes.push_back(RT);
  for (DIGlobalVariableExpression *GVE : CU->getGlobalVariables())
    CurrentDINodes.push_back(GVE->getVariable());
  for (DIImportedEntity *IE : CU->getImportedEntities())
    CurrentDINodes.push_back(IE);
  // Scan for nodes dependent on UserDINodes iteratively
  // until the set does not change anymore.
  bool Changed;
  do {
    UserVisited.clear();
    Changed = false;
    for (DINode *DIN : CurrentDINodes) {
      if (UserDINodes.count(DIN))
        continue;
      if (UserDIS.process(DIN))
        Changed = true;
    }
  } while (Changed);
  CurrentDINodes.clear();

#ifndef NDEBUG
  if (DINode *DIN = UserDIS.WhyThisDINode) {
    if (DISubprogram *DISP = dyn_cast<DISubprogram>(DIN)) {
      llvm::errs() << "==== User Function:.\n";
      DISP->dump();
    } else if (DIGlobalVariable *DIGV = dyn_cast<DIGlobalVariable>(DIN)) {
      llvm::errs() << "==== User GlobalVariable:.\n";
      DIGV->dump();
    }
    for (auto &I : TypeCache)
      if (DIN == cast_or_null<llvm::DIType>(I.second)) {
        QualType QT(QualType::getFromOpaquePtr(I.first));
        const Type *T = QT.getTypePtr();
        if (UD.Types.count(const_cast<Type *>(T))) {
          // WhyThisType = T;
          break;
        }
      }
  }
#endif

  // Finally, erase UserDINodes from all data structures.
  {
    typedef llvm::DenseMap<const void *, llvm::TrackingMDRef>::iterator
        Iterator;
    SmallVector<Iterator, 8> DeadTypes;
    for (Iterator I = TypeCache.begin(), E = TypeCache.end(); I != E; ++I) {
      DINode *DIN = cast_or_null<llvm::DIType>(I->second);
      QualType QT(QualType::getFromOpaquePtr(I->first));
      const Type *T = QT.getTypePtr();
      if (!T)
        continue;
      if (!DIN || UserDINodes.count(DIN) ||
          UD.Types.count(const_cast<Type *>(T)) ||
          TypesToKeepDeclaration.count(T))
        DeadTypes.push_back(I);
    }
    for (Iterator I : DeadTypes)
      TypeCache.erase(I);
  }
  {
    auto &RetainedTypes = CGDI->RetainedTypes;
    auto J = std::remove_if(
        RetainedTypes.begin(), RetainedTypes.end(),
        [&](void *Ty) { return TypeCache.find(Ty) == TypeCache.end(); });
    RetainedTypes.erase(J, RetainedTypes.end());
  }

  llvm::DIBuilder &DBuilder = CGDI->DBuilder;
  DBuilder.AllEnumTypes.clear();
  for (DICompositeType *ET : CU->getEnumTypes())
    if (!UserDINodes.count(ET))
      DBuilder.AllEnumTypes.push_back(ET);

  DBuilder.AllRetainTypes.clear();
  for (auto *RT : CU->getRetainedTypes())
    if (!UserDINodes.count(RT))
      DBuilder.AllRetainTypes.emplace_back(RT);

  DBuilder.AllSubprograms.clear();
#if 0
  // Processed through AllRetainTypes.
  for (DISubprogram *SP : CU->getSubprograms())
    if (!UserDINodes.count(SP))
      DBuilder.AllSubprograms.push_back(SP);
#endif

  DBuilder.AllGVs.clear();
  for (DIGlobalVariableExpression *GVE : CU->getGlobalVariables()) {
    if (!UserDINodes.count(GVE->getVariable()))
      DBuilder.AllGVs.push_back(GVE);
  }

  DBuilder.AllImportedModules.clear();
  for (DIImportedEntity *IE : CU->getImportedEntities())
    if (!UserDINodes.count(IE))
      DBuilder.AllImportedModules.emplace_back(IE);

  DBuilder.finalize();
  CU->replaceFilename("", "");

  SmallPtrSet<const MDNode *, 1> UserMDNodes(UserDINodes.begin(),
                                             UserDINodes.end());
  auto &DistinctMDNodes = M->getContext().pImpl->DistinctMDNodes;
  auto I = std::remove_if(DistinctMDNodes.begin(), DistinctMDNodes.end(),
                          [&](MDNode *MDN) { return UserMDNodes.count(MDN); });
  DistinctMDNodes.erase(I, DistinctMDNodes.end());

  for (const DINode *DIN : UserDINodes) {
    switch (DIN->getMetadataID()) {
#define HANDLE_MDNODE_LEAF_UNIQUABLE(CLASS)                                    \
  case Metadata::MetadataKind::CLASS##Kind: {                                  \
    CLASS *D = cast<CLASS>(const_cast<DINode *>(DIN));                         \
    M->getContext().pImpl->CLASS##s.erase(D);                                  \
    break;                                                                     \
  }
#include "llvm/IR/Metadata.def"
    }
  }

  for (const Decl *D : UD.Decls) {
    // ObjCInterfaceCache - Object C only.
    // ReplaceMap processed in finalize().
    // LexicalBlockStack cleared in finalize.
    CGDI->RegionMap.erase(D);
    // FnBeginRegionCount cleared in finalize.
    // DIFileCache handled by eraseFromDIFileCache called from
    // unloadDebugInfoHelper.
    if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D))
      CGDI->SPCache.erase(FD);
    else if (const NamespaceDecl *NSD = dyn_cast<NamespaceDecl>(D))
      CGDI->NamespaceCache.erase(NSD);
    else if (const NamespaceAliasDecl *NAD = dyn_cast<NamespaceAliasDecl>(D))
      CGDI->NamespaceAliasCache.erase(NAD);
    else if (const VarDecl *VD = dyn_cast<VarDecl>(D))
      CGDI->StaticDataMemberCache.erase(VD);
    else
      CGDI->DeclCache.erase(D);
  }
}
#if 0
static GlobalVariable *findGV(Constant *C) {
  if (GlobalVariable *GV = dyn_cast<GlobalVariable>(C))
    return GV;
  for (Use &U : C->operands())
    if (Constant *Op = dyn_cast<Constant>(U))
      if (GlobalVariable *GV = dyn_cast<GlobalVariable>(Op))
        return GV;
  return nullptr;
}
#endif

void Unloader::eraseMetadata() {
  for (StringRef Name :
       {"llvm.bitsets", "llvm.ident", "llvm.module.flags", "omp_offload.info"})
    if (llvm::NamedMDNode *NMD = M->getNamedMetadata(Name))
      M->eraseNamedMetadata(NMD);
#if 0
  // Not required as we reset before asan compilations.
  if (llvm::NamedMDNode *Globals = M->getNamedMetadata("llvm.asan.globals")) {
    SmallVector<MDNode *, 8> NewGlobals;
    for (MDNode *MDN : Globals->operands()) {
      Constant *C = mdconst::extract_or_null<Constant>(MDN->getOperand(0));
      if (!C)
        continue;
      GlobalVariable *GV = findGV(C);
      if (!GV || ConstantsToEraseCompletely.count(GV) ||
          GVsToKeepDeclaration.count(GV))
        continue;
      NewGlobals.push_back(MDN);
    }
    if (Globals->getNumOperands() != NewGlobals.size()) {
      Globals->dropAllReferences();
      for (MDNode *MDN : NewGlobals)
        Globals->addOperand(MDN);
    }
  }
#endif
}

} // namespace clang
