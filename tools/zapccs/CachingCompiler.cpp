// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "CachingCompiler.h"
#include "CachingPassManager.h"
#include "CommandLine.h"
#include "DependencyMap.h"
#include "GetMemoryUsage.h"
#include "Getters.h"
#include "StopWatch.h"
#include "StringConst.h"
#include "UnloadSelector.h"
#include "ZapccConfig.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Signals.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTMutationListener.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/FrontendTool/Utils.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Sema.h"

#define DEBUG_TYPE "zapcc"

using namespace llvm;

#ifndef NDEBUG
// Debug method to dump module on error from anywhere.
void dumpModule(Module *TheModule) {
  if (!TheModule)
    return;
  std::error_code EC;
  SmallString<1024> DumpName;
  sys::fs::createTemporaryFile("DumpModule", "txt", DumpName);
  errs() << "Dumping module into: " << DumpName << '\n';
  raw_fd_ostream dump(DumpName, EC, sys::fs::F_None);
  if (EC) {
    errs() << "error: dump file '" << DumpName << "' " << EC.message() << ".\n";
    return;
  }
  TheModule->setAlive();
  TheModule->CachingMode = false;
  TheModule->print(dump, nullptr);
  TheModule->resetAlive();
  TheModule->CachingMode = true;
}
#endif

namespace clang {

void CachingCompiler::init() {
  CPM.reset(new CachingPassManager(CI.get(), DM.get()));
  CI->CachingEmitObj = std::bind(&CachingPassManager::emitObj, CPM.get(),
                                 std::placeholders::_1, std::placeholders::_2);
  CI->CachingIncludedCallBack =
      std::bind(&CachingCompiler::included, this, std::placeholders::_1);
  CI->DiagsOS = &DiagsStream;
  CI->PreprocessorCreatedCallBack =
      std::bind(&CachingCompiler::PreprocessorCreatedCallBack, this,
                std::placeholders::_1);
}

CachingCompiler::CachingCompiler(int Id, const char *Argv0, void *MainAddr)
    : DiagsStream(DiagsString), ServerId(Id), Argv0(Argv0), MainAddr(MainAddr) {
  ZC.reset(new ZapccConfig);
  ZC->readConfigFile(Argv0);
  US->reset(*ZC);
  const char *ZAPCC_COMMANDS = getenv("ZAPCC_COMMANDS");
  PrintCommandsTxt = ZAPCC_COMMANDS;
  reset(false);
}

llvm::raw_ostream &CachingCompiler::printServerId(llvm::raw_ostream &OS) {
  OS << "(" << ServerId << ") ";
  return OS;
}

void CachingCompiler::printCommandsTxtName(StringRef What) {
  if (CommandsTxtName.size())
    printServerId(outs()) << What << " commands file at '" << CommandsTxtName
                          << "'\n";
}

void CachingCompiler::reset(bool IsError) {
  if (CommandsTxtName.size())
    CommandsTxtName.clear();
#ifndef NDEBUG
  if (CI.get()) {
    raw_ostream &OS = IsError ? errs() : outs();
    printServerId(OS) << "Cache cleared.\n";
  }
#endif
  CPM.reset(nullptr);
  CI.reset(new CompilerInstance);
  DM.reset(new DependencyMap);
  DiagsClient.reset(nullptr);
  LastCompileEndTime = 0;
}

CachingCompiler::~CachingCompiler() {}

bool CachingCompiler::compile(StringRef WorkingDir,
                              MutableArrayRef<const char *> Argv) {
  if (PrintCommandsTxt) {
    std::error_code EC;
    if (!CommandsTxtName.size()) {
      EC = sys::fs::createTemporaryFile("commands", "txt", CommandsTxtName);
      if (EC) {
        errs() << "error: while creating commands file '" << CommandsTxtName
               << "' " << EC.message() << ".\n";
        return false;
      }
    }
    raw_fd_ostream CommandsTxt(CommandsTxtName, EC, sys::fs::F_Append);
    if (EC) {
      errs() << "error: commands file '" << CommandsTxtName << "' "
             << EC.message() << ".\n";
      return false;
    }
    CommandsTxt << WorkingDir << "\n";
    for (StringRef Arg : Argv)
      CommandsTxt << Arg << "\n";
    CommandsTxt << "\n";
  }

  DEBUG_WITH_TYPE("server-files",
                  printServerId(errs()) << "Compiling " << Argv.back() << "\n");
  resetNumOccurrencesFlags();
  // Copied from tools/clang/tools/driver/cc1_main.cpp.
  IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
  // Buffer diagnostics from argument parsing so that we can output them using a
  // well formed diagnostic object.
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  TextDiagnosticBuffer *DiagsBuffer = new TextDiagnosticBuffer;
  DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagsBuffer);
  CI->getInvocation().resetOpts();
  bool Success = CompilerInvocation::CreateFromArgs(
      CI->getInvocation(), Argv.begin(), Argv.end(), Diags);
  // Infer the builtin include path if unspecified.
  auto &HSO = CI->getHeaderSearchOpts();
  if (HSO.UseBuiltinIncludes && HSO.ResourceDir.empty())
    HSO.ResourceDir = CompilerInvocation::GetResourcesPath(Argv0, MainAddr);

  // createDiagnostics() must be called after CreateFromArgs so that
  // CI->getDiagnosticOpts() will reflect the command line arguments.
  // We can't just reset to new Diagnostics since Sema has a pointer.
  if (CI->hasDiagnostics()) {
    CI->getDiagnostics().Reset();
    CI->getDiagnostics().getClient()->clear();
    ProcessWarningOptions(CI->getDiagnostics(), CI->getDiagnosticOpts());
  } else {
    DiagsClient.reset(
        new TextDiagnosticPrinter(DiagsStream, &CI->getDiagnosticOpts()));
    CI->createDiagnostics(DiagsClient.get(), false);
  }
  DiagsBuffer->FlushDiagnostics(CI->getDiagnostics());
  if (!Success)
    return false;
  CI->getFrontendOpts().DisableFree = false;
  // init must be called after CreateFromArgs.
  LangOptions &LangOpts = CI->getLangOpts();
  LangOpts.CachingMode = true;
  if (!CI->hasSourceManager())
    init();
  {
    StopWatch SW("Unload");
    unload();
  }
  DEBUG(dumpModule(getModule(CI.get())));
  {
    StopWatch SW("Compile");
    Success = ExecuteCompilerInvocation(CI.get());
  }
  DEBUG(dumpModule(getModule(CI.get())));
  if (Success)
    sys::ClearRemoveFileOnSignal();
  else
    return false;
  if (!CI->hasSourceManager() || !CI->hasPreprocessor())
    return false;
#ifndef NDEBUG
  for (auto &Pair : DM->FDMapping)
    assert(Pair.first->getDeclContext());
#endif
  LastCompileEndTime = std::time(nullptr);
  return true;
}

void CachingCompiler::included(const FileEntry *Includee) {
  if (!CI->hasSema())
    return;
  Sema &S = CI->getSema();
  {
    auto I = S.CachedPendingInstantiations.find(Includee);
    if (I != S.CachedPendingInstantiations.end())
      for (auto Inst : I->second)
        S.PendingInstantiations.push_back(Inst);
  }
  {
    // Mark live globals, based on included files.
    ASTContext &Context = S.getASTContext();
    auto I = DM->FDMapping.find(Includee);
    if (I != DM->FDMapping.file_end())
      for (const Decl *D : I->second)
        Context.LiveDecls.insert(D);
  }
}

bool CachingCompiler::isOutOfMemory() {
  unsigned MemoryUsage = getMemoryUsage();
  bool OutOfMemory = MemoryUsage > ZC->getMaxMemory();
#ifndef NDEBUG
  if (OutOfMemory)
    printServerId(llvm::outs())
        << "Memory usage is " << MemoryUsage << "MB, more than the "
        << ZC->getMaxMemory() << "MB limit, restarting.\n";
#endif
  return OutOfMemory;
}

void CachingCompiler::printDiags(bool AlwaysPrint) {
  StringRef D = DiagsStream.str();
  if (AlwaysPrint) {
    llvm::errs() << D;
    return;
  }
  if (!D.size())
    return;
  size_t MacroError = D.find("when including");
  if (MacroError != llvm::StringRef::npos) {
    llvm::errs() << "note: " << D.substr(MacroError);
  } else {
#ifndef NDEBUG
    llvm::errs() << "==== Suppressed diagnostics =========================\n";
    llvm::errs() << D;
    llvm::errs() << "=====================================================\n";
#endif
  }
}

void CachingCompiler::PreprocessorCreatedCallBack(Preprocessor &PP) {
  ZapccConfig::PatternsType &WatchMacro = ZC->getPatterns("[WatchMacro]");
  for (StringRef Name : WatchMacro)
    PP.CommandLineMainIdentifiersIdentifiers[PP.getIdentifierInfo(Name)] = macroUndefined;
}

class UpdateMutatedDeclsListener : public ASTMutationListener {
  void CompletedImplicitDefinition(const FunctionDecl *D) override {
    Decl::insertModifiedDecls(nullptr, const_cast<FunctionDecl *>(D));
  }
};

class UpdateMutatedDecls : public ASTConsumer {
  std::unique_ptr<UpdateMutatedDeclsListener> L =
      llvm::make_unique<UpdateMutatedDeclsListener>();

public:
  ASTMutationListener *GetASTMutationListener() override { return L.get(); }
};

class UpdateMutatedDeclsAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return llvm::make_unique<UpdateMutatedDecls>();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  ActionType getActionType() override { return AddBeforeMainAction; }
};

static FrontendPluginRegistry::Add<UpdateMutatedDeclsAction>
    X("UpdateMutatedDecls", "");
}
