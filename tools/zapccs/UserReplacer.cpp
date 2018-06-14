#include "UserReplacer.h"
#include "ReplaceDeclInMap.h"

#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/ExprCXX.h"

namespace clang {
#if 0
void UserReplacer::replace() {
  for (Decl *D : Context.NewDecls)
    replaceDecl(D);
  for (Type *T : Context.NewTypes)
    replaceType(T);
  for (NestedNameSpecifier *NNS : Context.NewNestedNameSpecifiers)
    replaceNestedNameSpecifier(NNS);
}
#endif

void UserReplacer::replaceDecl(Decl *D) {
  if (FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
    replaceStmt(FD->getBody());
    if (const CXXConstructorDecl *CXXCD = dyn_cast<CXXConstructorDecl>(FD))
      for (CXXCtorInitializer *CXXCI : CXXCD->inits())
        replaceStmt(CXXCI->getInit());
    else if (CXXDestructorDecl *CXXDD = dyn_cast<CXXDestructorDecl>(D))
      CXXDD->setRawOperatorDelete(nonUser(CXXDD->getRawOperatorDelete()));
  } else if (UsingDirectiveDecl *UDD = dyn_cast<UsingDirectiveDecl>(D)) {
    UDD->setRawNominatedNamespace(
        nonUser(UDD->getNominatedNamespaceAsWritten()));
  } else if (VarDecl *VRD = dyn_cast<VarDecl>(D)) {
    replaceStmt(VRD->getInit());
  } else if (LabelDecl *LD = dyn_cast<LabelDecl>(D)) {
    replaceStmt(LD->getStmt());
  }

#if 0
  } else if (auto *CTSD = dyn_cast<ClassTemplateSpecializationDecl>(D)) {
    if (CTSD->getTemplateSpecializationKind() !=
        TSK_ExplicitInstantiationDefinition)
      return;
    DeclContext *DC = CTSD->getDeclContext();
    DeclContext *LexicalDC = CTSD->getLexicalDeclContext();
    if (dyn_cast<Decl>(DC) != Dependee && dyn_cast<Decl>(LexicalDC) != Dependee)
      return;
    // If not found in DeclContext, abort. This may happen with:
    // namespace Bug1494m {
    // template <typename> class LockStats {
    //  template <typename> friend class LockStats;
    //};
    //}
    ReplaceDeclInMapImpl(CTSD, nullptr, DC);
    Replaced = true;
    DeclContext *ReplacementDC = cast<DeclContext>(Replacement);
    CTSD->setDeclContext(ReplacementDC);
    ReplacementDC->addDecl(CTSD);
    CTSD->setTemplateKeywordLoc(Replacement->getLocStart());
  }
#endif
}

void UserReplacer::replaceStmt(Stmt *S) {
  if (!S)
    return;
  if (Expr *E = dyn_cast<Expr>(S))
    replaceExpr(E);
  for (Stmt *Child : S->children())
    replaceStmt(Child);
}

void UserReplacer::replaceExpr(Expr *E) {
  switch (E->getStmtClass()) {
  case Stmt::StmtClass::DeclRefExprClass: {
    DeclRefExpr *DRE = cast<DeclRefExpr>(E);
    DRE->setDecl(nonUser(DRE->getDecl()));
    break;
  }
  case Stmt::StmtClass::MemberExprClass: {
    MemberExpr *ME = cast<MemberExpr>(E);
    ME->setMemberDecl(nonUser(ME->getMemberDecl()));
    break;
  }
#if 0
  case Stmt::StmtClass::AddrLabelExprClass: {
    AddrLabelExpr *ALE = cast<AddrLabelExpr>(E);
    ALE->setLabel(nonUser(ALE->getLabel()));
    break;
  }
  case Stmt::StmtClass::BlockExprClass: {
    BlockExpr *BE = cast<BlockExpr>(E);
    BE->setBlockDecl(nonUser(BE->getBlockDecl()));
    break;
  }
  case Stmt::StmtClass::MSPropertyRefExprClass: {
    MSPropertyRefExpr *MSPRE = cast<MSPropertyRefExpr>(E);
    MSPRE->setPropertyDecl(nonUser(MSPRE->getPropertyDecl()));
    break;
  }
#endif
  case Stmt::StmtClass::CXXDefaultArgExprClass: {
    CXXDefaultArgExpr *CXXDAE = cast<CXXDefaultArgExpr>(E);
    CXXDAE->setParam(nonUser(CXXDAE->getParam()));
    break;
  }
#if 0
  case Stmt::StmtClass::CXXDefaultInitExprClass: {
    CXXDefaultInitExpr *CXXDIE = cast<CXXDefaultInitExpr>(E);
    CXXDIE->setField(nonUser(CXXDIE->getField()));
    break;
  }
  case Stmt::StmtClass::CXXBindTemporaryExprClass: {
    CXXBindTemporaryExpr *CXXBTE = cast<CXXBindTemporaryExpr>(E);
    CXXTemporary *CXXT = CXXBTE->getTemporary();
    CXXT->setDestructor(
        nonUser(const_cast<CXXDestructorDecl *>(CXXT->getDestructor())));
    break;
  }
#endif
  case Stmt::StmtClass::CXXConstructExprClass:
  case Stmt::StmtClass::CXXTemporaryObjectExprClass: {
    CXXConstructExpr *CXXCE = cast<CXXConstructExpr>(E);
    CXXCE->setConstructor(nonUser(CXXCE->getConstructor()));
    break;
  }
  case Stmt::StmtClass::CXXNewExprClass: {
    CXXNewExpr *CXXNE = cast<CXXNewExpr>(E);
    CXXNE->setOperatorNew(nonUser(CXXNE->getOperatorNew()));
    CXXNE->setOperatorDelete(nonUser(CXXNE->getOperatorDelete()));
    break;
  }
  case Stmt::StmtClass::CXXDeleteExprClass: {
    CXXDeleteExpr *CXXDE = cast<CXXDeleteExpr>(E);
    CXXDE->setOperatorDelete(nonUser(CXXDE->getOperatorDelete()));
    break;
  }
#if 0
  case Stmt::StmtClass::UnresolvedLookupExprClass:
  case Stmt::StmtClass::UnresolvedMemberExprClass: {
    OverloadExpr *OE = cast<OverloadExpr>(E);
    if (UnresolvedLookupExpr *ULE = dyn_cast<UnresolvedLookupExpr>(E))
      ULE->setNamingClass(nonUser(ULE->getNamingClass()));
    break;
  }
  case Stmt::StmtClass::SizeOfPackExprClass: {
    SizeOfPackExpr *SOPE = cast<SizeOfPackExpr>(E);
    SOPE->setPack(nonUser(SOPE->getPack()));
    break;
  }
  case Stmt::StmtClass::SubstNonTypeTemplateParmPackExprClass: {
    SubstNonTypeTemplateParmPackExpr *SNTTPPE =
        cast<SubstNonTypeTemplateParmPackExpr>(E);
    SNTTPPE->setParameterPack(nonUser(SNTTPPE->getParameterPack()));
    break;
  }
  case Stmt::StmtClass::SubstNonTypeTemplateParmExprClass: {
    SubstNonTypeTemplateParmExpr *SNTTPE =
        cast<SubstNonTypeTemplateParmExpr>(E);
    SNTTPE->setParameter(nonUser(SNTTPE->getParameter()));
    break;
  }
  case Stmt::StmtClass::FunctionParmPackExprClass: {
    FunctionParmPackExpr *FPPE = cast<FunctionParmPackExpr>(E);
    FPPE->setParameterPack(nonUser(FPPE->getParameterPack()));
    break;
  }
#endif
  default: { break; }
  }
}

void UserReplacer::replaceType(Type *T) {
  switch (T->getTypeClass()) {
  case Type::TypeClass::VariableArray:
    replaceStmt(cast<VariableArrayType>(T)->getSizeExpr());
    break;
  case Type::TypeClass::DependentSizedArray:
    // Stored in FoldingSet.
    if (T->isCanonicalUnqualified())
      break;
    replaceStmt(cast<DependentSizedArrayType>(T)->getSizeExpr());
    break;
  case Type::TypeClass::DependentSizedExtVector:
    // Stored in FoldingSet.
    if (T->isCanonicalUnqualified())
      break;
    replaceStmt(cast<DependentSizedExtVectorType>(T)->getSizeExpr());
    break;
  case Type::TypeClass::UnresolvedUsing: {
    UnresolvedUsingType *UUT = cast<UnresolvedUsingType>(T);
    UUT->setDecl(nonUser(UUT->getDecl()));
    break;
  }
  case Type::TypeClass::Typedef: {
    TypedefType *TT = cast<TypedefType>(T);
    TT->setDecl(nonUser(TT->getDecl()));
    break;
  }
  case Type::TypeClass::TypeOfExpr: {
    // Stored in FoldingSet.
    if (T->isCanonicalUnqualified() && T->isDependentType())
      break;
    TypeOfExprType *TOET = cast<TypeOfExprType>(T);
    replaceStmt(TOET->getUnderlyingExpr());
    break;
  }
  case Type::TypeClass::Decltype: {
    // Stored in FoldingSet.
    if (T->isCanonicalUnqualified() && T->isDependentType())
      break;
    DecltypeType *DT = cast<DecltypeType>(T);
    replaceStmt(DT->getUnderlyingExpr());
    break;
  }
  case Type::TypeClass::Enum:
  case Type::TypeClass::Record: {
    TagType *TT = cast<TagType>(T);
    TT->setRawDecl(nonUser(TT->getRawDecl()));
    break;
  }
  case Type::TypeClass::TemplateTypeParm: {
    // Stored in FoldingSet.
    if (T->isCanonicalUnqualified())
      break;
    TemplateTypeParmType *TTPT = cast<TemplateTypeParmType>(T);
    TemplateTypeParmDecl *TTPD = TTPT->getDecl();
    TTPT->setDecl(nonUser(TTPD));
    break;
  }
  case Type::TypeClass::TemplateSpecialization: {
    // Stored in FoldingSet.
    if (T->isCanonicalUnqualified())
      break;
    TemplateSpecializationType *TST = cast<TemplateSpecializationType>(T);
    TemplateName &TN = TST->getTemplateName();
    if (TemplateDecl *TD = TN.getAsTemplateDecl())
      TN.setTemplateDecl(nonUser(TD));
    break;
  }
  case Type::TypeClass::InjectedClassName: {
    InjectedClassNameType *ICNT = cast<InjectedClassNameType>(T);
    ICNT->setRawDecl(nonUser(ICNT->getRawDecl()));
    break;
  }
  default: { break; }
  } // switch
}

void UserReplacer::replaceNestedNameSpecifier(NestedNameSpecifier *NNS) {
  NNS->setRawDecl(nonUser(NNS->getRawDecl()));
}
}