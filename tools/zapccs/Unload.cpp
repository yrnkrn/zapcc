// Copyright (c) 2014-2015 Ceemple Software Ltd. All rights Reserved.
#include "CachingCompiler.h"
#include "CachingPassManager.h"
#include "DependencyMap.h"
#include "Getters.h"
#include "StopWatch.h"
#include "UnloadSelector.h"
#include "Unloader.h"

#include "llvm/Support/Debug.h"

#include "clang/AST/ASTContext.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"

#define DEBUG_TYPE "zapcc-unload"

namespace clang {

using namespace llvm;

#if 0
// Implicit instantiations will usually instantiate at the end of
// translation unit by Sema::ActOnEndOfTranslationUnit(), unless
// they can not be deferred and even then CodeGenModule::EmitGlobal
// will delay the actual code generation.
// Either way, after the translation unit is done codegen will happen.
//
// In a following compilation, if explicit instantiation definition is
// found, clang will assume that this implicit instantiation had not
// generated any code yet and will codegen the instantiation again,
// leading to err_duplicate_mangled_name error in the function
// CodeGenModule::EmitGlobalFunctionDefinition() and an assert
// "Function already has body?" in CodeGenFunction::StartFunction().
//
// Explicit instantiation declaration will instantiate inline functions
// to IR but not MC (see ASTContext.cpp/basicGVALinkageForFunction()) due
// to C++11 [temp.explicit]p10:
//   [ Note: The intent is that an inline function that is the subject of
//   an explicit instantiation declaration will still be implicitly
//   instantiated when used so that the body can be considered for
//   inlining, but that no out-of-line copy of the inline function would
//   be generated in the translation unit. -- end note ]
// If an explicit instantiation definition is found in following
// compilation, it will instantiate the inline functions, also causing
// err_duplicate_mangled_name error.
//
// To handle these cases, we do not assert on existing body for functions
// in CodeGenModule::EmitGlobalFunctionDefinition() and
// CodeGenFunction::StartFunction(), instead deleting the existing body
// and going on with the new one.
//
// In addition, we allow two explicit instantiation definitions. While not
// allowed under C++ rules, it is a warning in Visual C++ mode and allowed
// in CachingMode. See Sema::CheckSpecializationInstantiationRedecl().
//
// Unintantiate only after function definitions are possibly unloaded.
template <class T> void resetInstantiationVar(T *V) {
  TemplateSpecializationKind Kind = V->getTemplateSpecializationKind();
  if (Kind == TSK_ImplicitInstantiation || Kind == TSK_ExplicitSpecialization) {
    V->setTemplateSpecializationKind(TSK_ImplicitInstantiation);
    V->setPointOfInstantiation(SourceLocation());
  }
}

template <class T> void resetInstantiationFunction(T *V) {
  TemplateSpecializationKind Kind = V->getTemplateSpecializationKind();
  if (Kind == TSK_ImplicitInstantiation || Kind == TSK_ExplicitSpecialization)
    V->setPointOfInstantiation(SourceLocation());
}

#endif

#ifndef NDEBUG
static void dumpDC(const Decl *D) {
  D->dump();
  if (const DeclContext *DC = dyn_cast<DeclContext>(D)) {
    puts("DeclContext:");
    DC->dumpDeclContext();
    puts("Lookups:");
    DC->dumpLookups();
    puts("");
  }
}

static void dump(ASTContext &Context, DependencyMap *DM) {
  puts("BEFORE UNLOAD:");
  dumpDC(Context.getTranslationUnitDecl());
  llvm::errs() << "\nUserFiles\n";
  for (const FileEntry *FE : DM->UD.FileEntries)
    llvm::errs() << FE << " " << FE->getName() << "\n";
#if 0
  llvm::errs() << "\nUserDecls\n";
  for (const Decl *D : DM->UD.Decls) {
    llvm::errs() << D << "\n";
  }
#else
  llvm::errs() << "\nUserDecls\n";
  for (const Decl *D : DM->UD.Decls) {
#if 0
    const DeclContext *DC = D->getDeclContext();
    if (const Decl *D = dyn_cast<Decl>(DC))
      if (DM->UD.Decls.count(D))
        continue;
#endif
    if (const TypeDecl *TD = dyn_cast<TypeDecl>(D))
      llvm::errs() << "Type=" << TD->getTypeForDecl() << "\n";
    if (const auto *CTSD = dyn_cast<ClassTemplateSpecializationDecl>(D))
      llvm::errs() << "SpecializationKind=" << CTSD->getSpecializationKind()
                   << "\n";
    if (const auto *FD = dyn_cast<FunctionDecl>(D))
      llvm::errs() << "TemplateSpecializationKind="
                   << FD->getTemplateSpecializationKind() << "\n";
    D->dump();
#if 0
    llvm::errs() << "Whose DeclContext is:\n";
    if (const Decl *D = dyn_cast<Decl>(DC))
      D->dumpColor();
    else
      llvm::errs() << DC;
#endif
    llvm::errs() << "\n";
  }
#if 1
  llvm::errs() << "\nUserTypes\n";
  for (const Type *T : DM->UD.Types) {
    T->dump();
    llvm::errs() << "\n";
  }
#endif
#endif
  llvm::errs() << "\n";
}

static void dumpAfter(ASTContext &Context) {
  puts("AFTER UNLOAD:");
  dumpDC(Context.getTranslationUnitDecl());
}

template <typename T> unsigned loc(SourceManager &SM, T V) {
  unsigned LOC = 0;
  for (const FileEntry *FE : V)
    if (const SrcMgr::ContentCache *CC = SM.getContentCache(FE))
      LOC += CC->NumLines;
  return LOC;
}

namespace {
class CacheReporter {
  Preprocessor &PP;
  UserData &UD;
  SourceManager &SM;
  unsigned Total;
  unsigned TotalLOC;

  void report(const char *Msg, unsigned Part, unsigned PartLOC) {
    char Buffer[128];
    snprintf(Buffer, sizeof(Buffer),
             "%8s %5u / %5u files (%3.0f%%) %8u / %8u LOC (%3.0f%%)\n", Msg,
             Part, Total, 100.0 * Part / Total, PartLOC, TotalLOC,
             100.0 * PartLOC / TotalLOC);
    llvm::errs() << Buffer;
  }

public:
  CacheReporter(Preprocessor &PP, UserData &UD)
      : PP(PP), UD(UD), SM(PP.getSourceManager()) {
    Total = PP.IncludeMgr.Includes.size();
    TotalLOC = loc(SM, PP.IncludeMgr.Includes);
  }
  void report() {
    llvm::errs() << "\ncache report\n============\n";
    report("total", Total, TotalLOC);
    unsigned Parsed = PP.ParsedFEs.size();
    unsigned ParsedLOC = loc(SM, PP.ParsedFEs);
    report("parsed", Parsed, ParsedLOC);
    unsigned Cached = Total - Parsed;
    unsigned CachedLOC = TotalLOC - ParsedLOC;
    report("cached", Cached, CachedLOC);
    unsigned Unloaded = UD.FileEntries.size();
    unsigned UnloadedLOC = loc(SM, UD.FileEntries);
    report("unloaded", Unloaded, UnloadedLOC);
    PP.ParsedFEs.clear();
  }
};
}

#endif

void CachingCompiler::unload() {
  if (!CI->hasSema())
    return;
  if (LastCompileEndTime) {
    double Seconds = std::difftime(std::time(nullptr), LastCompileEndTime);
    if (Seconds > 2) {
      StopWatch SW("findModifiedFiles");
      DM->findModifiedFiles(CI->getFileManager());
    }
  }
  ASTContext &Context = CI->getASTContext();
  Context.LiveDecls.clear();
  Preprocessor &PP = CI->getPreprocessor();
  DM->update(&Context, &PP);
  DM->collectUsers();
#ifndef NDEBUG
  auto &FEs = DM->UD.FileEntries;
  unsigned NonMutable = FEs.size() - DM->UD.MutableFileEntries;
  if (NonMutable > 3) {
    SourceManager &SM = PP.getSourceManager();
    StringRef FileName = SM.getFileEntryForID(SM.getMainFileID())->getName();
    llvm::errs() << "note: after compiling '" << FileName << "' unloaded "
                 << NonMutable << " non-mutable files\n";
  }
  DEBUG_WITH_TYPE("zapcc-files-list",
                  for (unsigned I = 0; I < FEs.size(); ++I) llvm::errs()
                      << FEs[I]->getName() << " U\n";);
#endif
  DEBUG(dump(Context, DM.get()););
  DEBUG_WITH_TYPE("zapcc-cache", CacheReporter CR(PP, DM->UD); CR.report(););
  Unloader U(CI->getSema(), getBuilder(CI.get()), DM->UD, DM->FDMapping);
  U.unload();
  DM->UD.clear();
  DEBUG(dumpAfter(Context););
} // CachingCompiler::unload

} // namespace ceemple
