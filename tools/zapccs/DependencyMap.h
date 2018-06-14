// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCCS_DEPENDENCYMAP_H
#define ZAPCCS_DEPENDENCYMAP_H

#include "clang/AST/DeclVisitor.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerUnion.h"

#include "FDMapping.h"
#include "UserData.h"

namespace clang {
class ASTContext;
class DeclContext;
class Expr;
class FileEntry;
class FileManager;
class QualType;
class OverloadExpr;
class Preprocessor;
class Stmt;
class TemplateArgument;
class TemplateArgumentList;
class TemplateDecl;
class TemplateName;
class TemplateParameterList;
class Type;
class TypeSourceInfo;

class DependencyMap : public DeclVisitor<DependencyMap, bool> {
public:
  DependencyMap() {}
  ~DependencyMap();
  void findModifiedFiles(FileManager &FM);
  void update(ASTContext *Ctx, Preprocessor *Pre);
  void collectUsers();
  bool dependsOnEmittedFunction(const VarDecl *VD);

  FileDeclsMapping FDMapping;
  UserData UD;

  bool VisitBlockDecl(const BlockDecl *BD);
  bool VisitCapturedDecl(const CapturedDecl *CD);
  bool VisitClassScopeFunctionSpecializationDecl(
      const ClassScopeFunctionSpecializationDecl *CSFSD);
  bool VisitDeclaratorDecl(const DeclaratorDecl *DD);
  bool VisitFieldDecl(const FieldDecl *FD);
  bool VisitFunctionDecl(const FunctionDecl *FD);
  bool VisitMSPropertyDecl(const MSPropertyDecl *MSPD);
  bool VisitNonTypeTemplateParmDecl(const NonTypeTemplateParmDecl *NTTPD);
  bool VisitVarDecl(const VarDecl *VRD);
  bool VisitParmVarDecl(const ParmVarDecl *PVD);
  bool
  VisitVarTemplateSpecializationDecl(const VarTemplateSpecializationDecl *VTSD);
  bool VisitDecompositionDecl(const DecompositionDecl *DD);
  bool VisitFriendDecl(const FriendDecl *FD);
  bool VisitLabelDecl(const LabelDecl *LD);
  bool VisitNamespaceAliasDecl(const NamespaceAliasDecl *NAD);
  bool VisitNamespaceDecl(const NamespaceDecl *NSD);
  bool VisitTemplateDecl(const TemplateDecl *TD);
  bool VisitRedeclarableTemplateDecl(const RedeclarableTemplateDecl *RTD);
  bool VisitFunctionTemplateDecl(const FunctionTemplateDecl *FTD);
  bool VisitTemplateTemplateParmDecl(const TemplateTemplateParmDecl *TTPD);
  bool VisitTypeDecl(const TypeDecl *TD);
  bool VisitTagDecl(const TagDecl *TD);
  bool VisitEnumDecl(const EnumDecl *ED);
  bool VisitRecordDecl(const RecordDecl *RD);
  bool VisitCXXRecordDecl(const CXXRecordDecl *CXXRD);
  bool VisitClassTemplateSpecializationDecl(
      const ClassTemplateSpecializationDecl *CTSD);
  bool VisitClassTemplatePartialSpecializationDecl(
      const ClassTemplatePartialSpecializationDecl *CTPSD);
  bool VisitTemplateTypeParmDecl(const TemplateTypeParmDecl *TTPD);
  bool VisitTypedefNameDecl(const TypedefNameDecl *TND);
  bool VisitTypedefDecl(const TypedefDecl *TD);
  bool VisitTypeAliasDecl(const TypeAliasDecl *TAD);
  bool
  VisitUnresolvedUsingTypenameDecl(const UnresolvedUsingTypenameDecl *UUTD);
  bool VisitUsingDecl(const UsingDecl *UD);
  bool VisitUsingPackDecl(const UsingPackDecl *UPD);
  bool VisitUsingShadowDecl(const UsingShadowDecl *USD);
  bool VisitUsingConstructorUsingShadowDecl(ConstructorUsingShadowDecl *CUSD);
  bool VisitUsingDirectiveDecl(const UsingDirectiveDecl *UDD);
  bool VisitBindingDecl(const BindingDecl *BD);
  bool VisitEnumConstantDecl(const EnumConstantDecl *ECD);
  bool VisitIndirectFieldDecl(const IndirectFieldDecl *IFD);
  bool VisitUnresolvedUsingValueDecl(const UnresolvedUsingValueDecl *UUVD);
  bool VisitStaticAssertDecl(const StaticAssertDecl *SAD);

private:
  DependencyMap(DependencyMap &) = delete;
  DependencyMap &operator=(DependencyMap &) = delete;
  typedef llvm::PointerUnion4<Decl *, Type *, NestedNameSpecifier *,
                              FileEntry *>
      DependentType;
  llvm::SmallPtrSet<DependentType, 8> Visited;
  llvm::SmallPtrSet<DependentType, 8> VisitedUsers;
  llvm::SmallPtrSet<FunctionDecl *, 8> VisitedDepend;
#ifndef NDEBUG
  llvm::SmallVector<DependentType, 16> DependentStack;
#endif
  DependentType CurrentDependent;
  // Dependents depend on key (Dependee).
  llvm::DenseMap<void *, llvm::SmallPtrSet<DependentType, 4>> Dependents;
  // Key uses (depends) on the set.
  llvm::DenseMap<const FileEntry *, llvm::SmallPtrSet<const FileEntry *, 4>>
      FileEntryDependees;
  // Types that need new canonical since their canonical was unloaded.
  llvm::SmallPtrSet<Type *, 8> RecanonTypes;
  bool RemoveDependencies = false;
  ASTContext *Context = nullptr;
  Preprocessor *PP = nullptr;

  bool findProlog(DependentType NewDependee, DependentType &LastDependent);
  void collectUsersOf(DependentType DT);
  bool dependsOnEmittedFunctionOf(FunctionDecl *FD);
  void print(DependentType DT);

  void findInDecl(const Decl *cD);
  void findInNestedNameSpecifier(const NestedNameSpecifier *cNNS);
  void findInType(Type *T);
  void findInType(const Type *T);
  void findInQualType(QualType QT);
  void findInQualTypeDefinition(QualType QT);
  void findInTypeSourceInfo(const TypeSourceInfo *TSI);
  void findInTypeSourceInfoDefinition(const TypeSourceInfo *TSI);
  void findInTemplateArgument(const TemplateArgument *cTA);
  void findInTemplateArgumentList(const TemplateArgumentList &TAL);
  void findInTemplateName(TemplateName TN);
  void findInTemplateParameterList(TemplateParameterList *TPL);
  void findInExpr(const Expr *E);
  void findInStmt(const Stmt *S);
  void findInDeclContext(const DeclContext *DC) {
    findInDecl(dyn_cast<Decl>(DC));
  }
  void findInFileEntry(const FileEntry *FE);
  void findInFileEntry(FileEntry *FE);
};

} // namespace clang

#endif
