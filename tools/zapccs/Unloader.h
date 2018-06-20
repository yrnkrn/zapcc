// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ZAPCC_UNLOADER_H
#define ZAPCC_UNLOADER_H

#include "clang/AST/DeclVisitor.h"
#include "clang/AST/GlobalDecl.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalValue.h"

#include "UserData.h"

namespace llvm {
class Constant;
class Function;
class GlobalValue;
class Module;
class Type;
class StructType;
}

namespace clang {
class Decl;
class FileDeclsMapping;
class GlobalDecl;
class Sema;
namespace CodeGen {
class CGDebugInfo;
class CGOpenMPRuntime;
class CodeGenModule;
}

using llvm::SmallPtrSet;
using llvm::SmallPtrSetImpl;

class Unloader : public DeclVisitor<Unloader, bool> {
  SmallPtrSet<Decl *, 8> Visited;

  Sema &S;
  Preprocessor &PP;
  ASTContext &C;
  SourceManager &SM;
  CodeGen::CodeGenModule *Builder;
  UserData &UD;
  FileDeclsMapping &FDMapping;
  llvm::Module *M;

  bool UnloadDecl(Decl *D);
  // See UserReplace::nonUser
  template <typename T> T *nonUser(T *D) {
    T *NonUser = nullptr;
    T *R = D->getMostRecentDecl();
    while (R) {
      if (R != D && !UD.Decls.count(R))
        NonUser = R;
      R = dyn_cast_or_null<T>(R->getPreviousDecl());
    }
    return NonUser;
  }

  SmallPtrSet<const llvm::Type *, 1> ErasedTypes;
  SmallPtrSet<const Type *, 1> TypesToKeepDeclaration;
  llvm::SetVector<llvm::GlobalValue *> GVsToKeepDeclaration;
  llvm::SetVector<llvm::Constant *> ConstantsToEraseCompletely;
  SmallPtrSet<llvm::Constant *, 1> VisitedConstants;
#ifndef NDEBUG
  llvm::SmallVector<llvm::Constant *, 16> ConstantsStack;
#endif
  void removeDeclFromModule(const Decl *D);
  void removeNamedDeclFromModule(const NamedDecl *ND);
  bool insertConstantsToEraseCompletely(llvm::Constant *C);
  void insertCachedDeferredDeclsToEmit(llvm::GlobalValue *GV);
  void insertConstantAndUsers(llvm::Constant *C);
  void collectConstants();
  void eraseDefinition(llvm::GlobalValue *GV);
  void eraseConstantandUsers(llvm::Constant *C);
  void eraseBuilder();
  void eraseOpenMP(CodeGen::CGOpenMPRuntime *OpenMPRuntime);
  void eraseType(const Type *T, bool EraseDeclaration);
  void eraseCodeGenTypes();
  void eraseDeclsASTContext();
  void eraseTypesASTContext();
  void eraseNestedNameSpecifiersASTContext();
  void recanonTypes();
  void restoreWeakRef();
  void unloadVectors();
  void unloadDeclContexts();
  void eraseDeclContextsASTContext();
  void eraseDebugTypes(CodeGen::CGDebugInfo *CGDI);
  void eraseMetadata();
  void processInstantiationsChange();
  void eraseMacros();
  void eraseFileEntries();

public:
  Unloader(Sema &S, CodeGen::CodeGenModule *Builder, UserData &UD,
           FileDeclsMapping &FDMapping);
  ~Unloader() {}
  void unload();
  static void ReplaceDeclInMap(NamedDecl *OriginalND, NamedDecl *ReplacementND,
                               DeclContext *DC = nullptr);

  bool VisitDecl(Decl *D);
  //bool VisitFileScopeAsmDecl(FileScopeAsmDecl *FSAD);
  bool VisitFriendDecl(FriendDecl *FD);
  bool VisitNamedDecl(NamedDecl *ND);
  bool VisitNamespaceAliasDecl(NamespaceAliasDecl *NAD);
  bool VisitTypedefNameDecl(TypedefNameDecl *TND);
  bool VisitVarDecl(VarDecl *VD);
  bool VisitParmVarDecl(ParmVarDecl *PVD);
  // bool VisitTemplateTemplateParmDecl(TemplateTemplateParmDecl *TTPD);
  bool VisitFunctionDecl(FunctionDecl *FD);
  bool VisitLinkageSpecDecl(LinkageSpecDecl *LSD);
  bool VisitDeclContext(DeclContext *DC);
  bool VisitNamespaceDecl(NamespaceDecl *NSD);
  bool VisitBlockDecl(BlockDecl *BD);
  bool VisitTagDecl(TagDecl *TD);
  bool VisitEnumDecl(EnumDecl *ED);
  bool VisitEnumConstantDecl(EnumConstantDecl *ECD);
  bool VisitRecordDecl(RecordDecl *RD);
  bool VisitCXXRecordDecl(CXXRecordDecl *CXXRD);
  bool VisitUsingDecl(UsingDecl *UD);
  bool VisitUsingShadowDecl(UsingShadowDecl *USD);
  bool VisitUsingConstructorUsingShadowDecl(ConstructorUsingShadowDecl *CUSD);

  bool VisitTemplateDecl(TemplateDecl *TD);
  bool VisitRedeclarableTemplateDecl(RedeclarableTemplateDecl *RTD);
  bool VisitClassTemplateDecl(ClassTemplateDecl *CTD);
  bool VisitVarTemplateSpecializationDecl(VarTemplateSpecializationDecl *VTSD);
  bool
  VisitClassTemplateSpecializationDecl(ClassTemplateSpecializationDecl *CTSD);
};

template <typename T, typename Pred> void eraseDenseMap(T &Map, Pred pred) {
  for (auto I = Map.begin(), E = Map.end(); I != E;) {
    auto J = I;
    ++J;
    if (pred(*I))
      Map.erase(I);
    I = J;
  }
}
}
#endif
