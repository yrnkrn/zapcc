// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "FDMapping.h"
#include "Unloader.h"

#include "../lib/CodeGen/CGCXXABI.h"
#include "../lib/CodeGen/CodeGenTBAA.h"
#include "../lib/IR/LLVMContextImpl.h"
#include "../lib/Serialization/ASTCommon.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Sema.h"

using namespace llvm;
namespace clang {

/// NamespaceDecl, VarDecl, FunctionDecl, TypedefNameDecl,
/// TagDecl, UsingShadowDecl, RedeclarableTemplateDecl

template <typename T> bool VisitRedeclarable(Redeclarable<T> *R) {
  T *First = R->getFirstDecl();
  T *MostRecent = First->getMostRecentDecl();
  bool LastOne = (First == MostRecent);
  // Removed may be the first, latest, or in-between
  // If this is last one on chain, don't fix it.
  if (LastOne) {
  } else if (R == MostRecent) {
    // This also works with length 2 chain, will
    // make First point next to itself as it should.
    MostRecent = MostRecent->getPreviousDecl();
    First->setMostRecent(MostRecent);
  } else {
    // Find who points back at R.
    T *D = MostRecent;
    while (D->getPreviousDecl() != R)
      D = D->getPreviousDecl();
    if (R == First) {
      // D is the new First.
      First = D;
      First->setMostRecent(MostRecent);
      // Update first in whole chain according to r233228.
      D = MostRecent;
      while (D) {
        D->setFirstDecl(First);
        D = D->getPreviousDecl();
      }
    } else {
      // D will point to R previous, no change in First or MostRecent.
      D->setPrevious(R->getPreviousDecl());
    }
  }
  T *Erased = static_cast<T *>(R);
  // Disconnect Erased from redecl chain in case it's accessed for mangling.
  Erased->setMostRecent(Erased);
  Erased->setFirstDecl(Erased);
  NamedDecl *ErasedND = cast<NamedDecl>(Erased);
  if (LastOne) {
    Unloader::ReplaceDeclInMap(ErasedND, nullptr);
    return true;
  }
  NamedDecl *ReplacementND = cast<NamedDecl>(MostRecent);
  if (!ReplacementND) {
    Unloader::ReplaceDeclInMap(ErasedND, nullptr);
    return true;
  }
  // Replacement DeclContext must contain the Erased DeclContext, else
  // ReplacementND could never be found and removed later, when searching
  // from the Replacement DeclContext upwards. In this case we give up replacing
  // and just remove ErasedND.
  auto getBaseContext = [](NamedDecl *ND) {
    DeclContext *DC = ND->getDeclContext();
    while (isa<LinkageSpecDecl>(DC))
      DC = DC->getParent();
    DC = DC->getPrimaryContext();
    return DC;
  };
  DeclContext *ErasedDC = getBaseContext(ErasedND);
  DeclContext *ReplacementDC = getBaseContext(ReplacementND);
  bool ReplacementContainsErased = false;
  DeclContext *DC = ReplacementDC;
  do {
    DC = DC->getPrimaryContext();
    if (ErasedDC == DC) {
      ReplacementContainsErased = true;
      break;
    }
    DC = DC->getParent();
  } while (DC);
  if (!ReplacementContainsErased)
    ReplacementND = nullptr;
  Unloader::ReplaceDeclInMap(ErasedND, ReplacementND);
  return true;
}

bool Unloader::VisitDecl(Decl *D) {
  if (!Builder)
    return true;
  removeDeclFromModule(D);
  return true;
}
#if 0
bool Unloader::VisitFileScopeAsmDecl(FileScopeAsmDecl *FSAD) {
  if (Builder)
    Builder->getModule().eraseModuleInlineAsm(
        FSAD->getAsmString()->getString());
  VisitDecl(FSAD);
  return true;
}
#endif
bool Unloader::VisitFriendDecl(FriendDecl *FD) {
  VisitDecl(FD);
  return true;
}

static void eraseFromOverloadExprs(ASTContext &C, NamedDecl *ND) {
  auto I = C.OverloadExprs.find(ND);
  if (I == C.OverloadExprs.end())
    return;
  for (OverloadExpr *OE : I->second)
    OE->erase(ND);
  C.OverloadExprs.erase(I);
}

bool Unloader::VisitNamedDecl(NamedDecl *ND) {
  eraseFromOverloadExprs(S.getASTContext(), ND);
  VisitDecl(ND);
  if (Builder) {
    removeNamedDeclFromModule(ND);
    Builder->ErasedDeferredDeclsToEmit.erase(ND);
  }
  if (DeclContext *DC = ND->getDeclContext()) {
    DC = DC->getRedeclContext();
    if (Scope *Scp = S.getScopeForContext(DC))
      Scp->RemoveDecl(ND);
  }
  S.IdResolver.EraseDecl(ND);
  // NamedDecl which are Redeclarable will be removed
  // from map by VisitRedeclarable, so we skip them here.
  if (!serialization::isRedeclarableDeclKind(ND->getKind()))
    ReplaceDeclInMap(ND, nullptr);
  if (Builder)
    Builder->ABI->getMangleContext().eraseNamedDecl(ND);
  return true;
}

bool Unloader::VisitNamespaceAliasDecl(NamespaceAliasDecl *NAD) {
  // NamespaceAliasDecl: NamedDecl, Redeclarable
  VisitNamedDecl(NAD);
  VisitRedeclarable(NAD);
  return true;
}

bool Unloader::VisitTypedefNameDecl(TypedefNameDecl *TND) {
  // TypedefNameDecl: TypeDecl, Redeclarable
  VisitTypeDecl(TND);
  VisitRedeclarable(TND);
  return true;
}

template <typename SomeDecl>
void MaybeEraseStaticInExternC(
    CodeGen::CodeGenModule::StaticExternCMap &StaticExternCValues,
    const SomeDecl *D) {
  if (IdentifierInfo *II = D->getIdentifier()) {
    auto I = StaticExternCValues.find(II);
    if (I != StaticExternCValues.end())
      StaticExternCValues.erase(I);
  }
}

bool Unloader::VisitVarDecl(VarDecl *VD) {
  // VarDecl : DeclaratiorDecl, Redeclarable
  VisitDeclaratorDecl(VD);
  VisitRedeclarable(VD);
  if (VD->isExternC())
    ReplaceDeclInMap(VD, nullptr, C.getExternCContextDecl());
  if (Builder) {
    // Remove guard variable.
    auto J = Builder->StaticLocalDeclGuardMap.find(VD);
    if (J != Builder->StaticLocalDeclGuardMap.end()) {
      insertConstantsToEraseCompletely(J->second);
      Builder->StaticLocalDeclGuardMap.erase(J);
    }
    MaybeEraseStaticInExternC(Builder->StaticExternCValues, VD);
    Builder->GlobalDecls.erase(VD);
  }
  return true;
}

bool Unloader::VisitParmVarDecl(ParmVarDecl *PVD) {
  S.UnparsedDefaultArgInstantiations.erase(PVD);
  S.UnparsedDefaultArgLocs.erase(PVD);
  return VisitVarDecl(PVD);
}

bool Unloader::VisitFunctionDecl(FunctionDecl *FD) {
  // FunctionDecl : DeclaratiorDecl, DeclContext, Redeclarable
  VisitDeclContext(FD);
  VisitDeclaratorDecl(FD);
  VisitRedeclarable(FD);
  if (FD->isExternC())
    ReplaceDeclInMap(FD, nullptr, C.getExternCContextDecl());
  if (FD->isFunctionTemplateSpecialization() && FD->isCanonicalDecl()) {
    FunctionTemplateDecl *FTD =
        FD->getTemplateSpecializationInfo()->getTemplate();
    FTD->getSpecializations().erase(FD->getTemplateSpecializationInfo());
  }
  if (Builder) {
    MaybeEraseStaticInExternC(Builder->StaticExternCValues, FD);
    Builder->GlobalDecls.erase(FD);
  }
  return true;
}

bool Unloader::VisitLinkageSpecDecl(LinkageSpecDecl *LSD) {
  // LinkageSpecDecl: DeclContext, Decl (LinkageSpecDecl never in Builder)
  return VisitDeclContext(LSD);
}

bool Unloader::VisitDeclContext(DeclContext *DC) {
  for (Decl *D : DC->noload_decls())
    UnloadDecl(D);
  return true;
}

bool Unloader::VisitNamespaceDecl(NamespaceDecl *NSD) {
  // NamespaceDecl: NamedDecl, DeclContext, Redeclarable
  // Anonymous namespaces are stored in parent, not Map.
  // See Sema::ActOnStartNamespaceDef.
  S.KnownNamespaces.erase(NSD);
  if (S.StdExperimentalNamespaceCache == NSD)
    S.StdExperimentalNamespaceCache = nullptr;
  DeclContext *Parent = NSD->getDeclContext();
  Parent = Parent->getRedeclContext();
  if (TranslationUnitDecl *TUD = dyn_cast<TranslationUnitDecl>(Parent)) {
    if (NSD == TUD->getAnonymousNamespace())
      TUD->setAnonymousNamespace(NSD->getPreviousDecl());
  } else if (NamespaceDecl *ND = dyn_cast<NamespaceDecl>(Parent)) {
    if (NSD == ND->getAnonymousNamespace()) {
      NamespaceDecl *Prev = NSD->getPreviousDecl();
      ND->setAnonymousNamespace(Prev);
    }
  }
  VisitDeclContext(NSD);
  VisitNamedDecl(NSD);
  // Fix OriginalNamespace and move LookupPtr if required.
  bool IsStd = NSD == S.getStdNamespace();
  if (NSD->isFirstDecl()) {
    NamespaceDecl *Last = NSD->getMostRecentDecl();
    if (NSD != Last) {
      NamespaceDecl *Second = Last;
      // Find the second namespace in chain.
      while (Second->getPreviousDecl() != NSD)
        Second = Second->getPreviousDecl();
      if (IsStd)
        S.StdNamespace = Second;
      // Copy anonymous namespace, if any, from the first. We can't use
      // setAnonymousNamespace as Second is not (yet) the first decl.
      Second->setRawNamespace(NSD->getAnonymousNamespace());
      // Fix all following namespaces to point to the second.
      while (Last != Second) {
        Last->setRawNamespace(Second);
        Last = Last->getPreviousDecl();
      }
      if (StoredDeclsMap *Ptr = NSD->getLookupPtr())
        Second->setLookupPtr(Ptr);
      if (NSD->getHasLazyLocalLexicalLookups())
        Second->setHasLazyLocalLexicalLookups(true);
      NSD->setLookupPtr(nullptr);
    } else {
      if (IsStd)
        S.StdNamespace = nullptr;
    }
  } else if (IsStd) {
    // We know it's not the first.
    S.StdNamespace = NSD->getPreviousDecl();
  }
  VisitRedeclarable(NSD);
  return true;
}

bool Unloader::VisitTagDecl(TagDecl *TD) {
  // TagDecl: TypeDecl, DeclContext, Redeclarable
  TD->setCompleteDefinition(false);
  VisitDeclContext(TD);
  VisitTypeDecl(TD);
  VisitRedeclarable(TD);
  if (Builder)
    Builder->ABI->getMangleContext().eraseTagDecl(TD);
  return true;
}

bool Unloader::VisitEnumDecl(EnumDecl *ED) {
  // EnumDecl: TagDecl
  S.FlagBitsCache.erase(ED);
  if (S.StdAlignValT.get(nullptr) == ED)
    S.StdAlignValT = nullptr;
  return VisitTagDecl(ED);
}

bool Unloader::VisitEnumConstantDecl(EnumConstantDecl *ECD) {
  return VisitValueDecl(ECD);
}

bool Unloader::VisitRecordDecl(RecordDecl *RD) {
  // RecordDecl: TagDecl
  if (S.CXXTypeInfoDecl == RD)
    S.CXXTypeInfoDecl = nullptr;
  else if (S.MSVCGuidDecl == RD)
    S.MSVCGuidDecl = nullptr;
  if (RD->isCompleteDefinition()) {
    if (const Type *T = RD->getTypeForDecl())
      TypesToKeepDeclaration.insert(T);
  }
  VisitTagDecl(RD);
  return true;
}

bool Unloader::VisitCXXRecordDecl(CXXRecordDecl *CXXRD) {
  CXXRecordDecl *NewFirst = nullptr;
  CXXRecordDecl *NewDef = nullptr;
  CXXRecordDecl *R = CXXRD->getMostRecentDecl();
  do {
    if (!UD.Decls.count(R)) {
      NewFirst = R;
      if (R->isThisDeclarationADefinition())
        NewDef = R;
    }
    R = R->getPreviousDecl();
  } while (R);
  if (S.PureVirtualClassDiagSet)
    S.PureVirtualClassDiagSet->erase(CXXRD);
  if (S.StdBadAlloc.get(nullptr) == CXXRD)
    S.StdBadAlloc = nullptr;
  VisitRecordDecl(CXXRD);
  if (NewFirst)
    NewFirst->setDefinitionData(NewDef);
  if (Builder)
    Builder->ABI->getMangleContext().eraseCXXRecordDecl(CXXRD);
  return true;
}

bool Unloader::VisitBlockDecl(BlockDecl *BD) {
  // BlockDecl: DeclContext, Decl
  VisitDeclContext(BD);
  VisitDecl(BD);
  if (Builder)
    Builder->ABI->getMangleContext().eraseBlockDecl(BD);
  return true;
}

bool Unloader::VisitUsingDecl(UsingDecl *UD) {
  for (UsingShadowDecl *USD : UD->shadows()) {
    // Do not use UnloadDecl(USD) calling VisitUsingShadowDecl,
    // it removes USD from UD and messes up UD->shadows().
    VisitNamedDecl(USD);
    VisitRedeclarable(USD);
  }
  VisitNamedDecl(UD);
  return true;
}

bool Unloader::VisitUsingShadowDecl(UsingShadowDecl *USD) {
  // UsingShadowDecl: NamedDecl, Redeclarable
  VisitNamedDecl(USD);
  VisitRedeclarable(USD);
  UsingDecl *UD = USD->getUsingDecl();
  // Sema::HideUsingShadowDecl may have removed the UsingShadowDecl from
  // the UsingDecl list, so we use our own eraseShadowDecl instead of
  // removeShadowDecl which will assert in this situation.
  UD->eraseShadowDecl(USD);
  return true;
}

bool Unloader::VisitUsingConstructorUsingShadowDecl(
    ConstructorUsingShadowDecl *CUSD) {
  VisitUsingShadowDecl(CUSD);
  return true;
}

bool Unloader::VisitTemplateDecl(TemplateDecl *TD) {
  // TemplateDecl: NamedDecl
  // Remove previous default arguments inherited from us.
  for (auto *D : TD->redecls()) {
    TemplateDecl *R = cast<TemplateDecl>(D);
    if (R == TD || UD.Decls.count(R))
      continue;
    TemplateParameterList *TPL = TD->getTemplateParameters();
    TemplateParameterList *RTPL = R->getTemplateParameters();
    ArrayRef<NamedDecl *> Params = TPL->asArray();
    ArrayRef<NamedDecl *> RParams = RTPL->asArray();
    assert(Params.size() == RParams.size());
    for (unsigned i = 0, e = RParams.size(); i < e; ++i)
      if (auto *TTPD = dyn_cast<TemplateTypeParmDecl>(RParams[i]))
        if (TTPD->getDefaultArgStorage().getInheritedFrom() == Params[i])
          TTPD->removeDefaultArgument();
  }
  VisitNamedDecl(TD);
  return true;
}

bool Unloader::VisitRedeclarableTemplateDecl(RedeclarableTemplateDecl *RTD) {
  // RedeclarableTemplateDecl: TemplateDecl, Redeclarable
  VisitTemplateDecl(RTD);
  VisitRedeclarable(RTD);
  return true;
}

bool Unloader::VisitClassTemplateDecl(ClassTemplateDecl *CTD) {
  // ClassTemplateDecl: RedeclarableTemplateDecl
  if (S.StdInitializerList == CTD)
    S.StdInitializerList = nullptr;
  VisitRedeclarableTemplateDecl(CTD);
  return true;
}

bool Unloader::VisitVarTemplateSpecializationDecl(
    VarTemplateSpecializationDecl *VTSD) {
  // VarTemplateSpecializationDecl: VarDecl, FoldingSet
  VarTemplateDecl *VTD = VTSD->getSpecializedTemplate();
  VarTemplateSpecializationDecl *Replacement = nonUser(VTSD);
  if (Replacement) {
    auto I = C.InstantiationChanges.find(VTSD);
    if (I != C.InstantiationChanges.end())
      Replacement->setTemplateSpecializationKind(I->second.Old_TSK);
  }
  if (VarTemplatePartialSpecializationDecl *VTPSD =
          dyn_cast<VarTemplatePartialSpecializationDecl>(VTSD)) {
    VTD->getPartialSpecializations().erase(VTPSD);
    if (Replacement)
      VTD->getPartialSpecializations().InsertNode(
          cast<VarTemplatePartialSpecializationDecl>(Replacement));
  } else {
    VTD->getSpecializations().erase(VTSD);
    if (Replacement)
      VTD->getSpecializations().InsertNode(Replacement);
  }
  VisitVarDecl(VTSD);
  return true;
}

bool Unloader::VisitClassTemplateSpecializationDecl(
    ClassTemplateSpecializationDecl *CTSD) {
  // ClassTemplateSpecializationDecl: CXXRecordDecl, FoldingSet
  ClassTemplateDecl *CTD = CTSD->getSpecializedTemplate();
  ClassTemplateSpecializationDecl *Replacement = nonUser(CTSD);
  if (Replacement) {
    auto I = C.InstantiationChanges.find(CTSD);
    if (I != C.InstantiationChanges.end())
      Replacement->setTemplateSpecializationKind(I->second.Old_TSK);
  }
  if (ClassTemplatePartialSpecializationDecl *CTPSD =
          dyn_cast<ClassTemplatePartialSpecializationDecl>(CTSD)) {
    if (CTD->getPartialSpecializations().erase(CTPSD))
      if (Replacement)
        CTD->getPartialSpecializations().InsertNode(
            cast<ClassTemplatePartialSpecializationDecl>(Replacement));
  } else {
    if (CTD->getSpecializations().erase(CTSD))
      if (Replacement)
        CTD->getSpecializations().InsertNode(Replacement);
  }
  VisitCXXRecordDecl(CTSD);
  return true;
}

bool Unloader::UnloadDecl(Decl *D) {
#if 0
  if (D->ignoreGeneratedDecl())
    return true;
#endif
  if (!Visited.insert(D).second)
    return true;
  assert(UD.Decls.count(D));
  Visit(D);
  FDMapping.erase(D);
  return true;
}

static bool isSameKind(const Decl *Replacement, const GlobalValue *GV) {
  if (!Replacement)
    return false;
  if (isa<FunctionDecl>(Replacement) && isa<Function>(GV))
    return true;
  return isa<VarDecl>(Replacement) && isa<GlobalVariable>(GV);
}

void Unloader::removeDeclFromModule(const Decl *D) {
  const Decl *Replacement;
  bool NeedDefinition;
  if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
    bool WasDefinition = FD->isThisDeclarationADefinition();
    // Don't keep IR declarations for function definitions in record
    // lexical context. See: CodeGenModule::GetOrCreateLLVMFunction.
    if (WasDefinition && isa<CXXRecordDecl>(FD->getLexicalDeclContext()))
      FD = nullptr;
    else
      FD = nonUser(FD);
    Replacement = FD;
    // Replacement may be a definition, such as multi/function-def-use.
    // Amazingly enough, with __attribute__((gnu_inline)) we may find
    // two different definitions, one for inline and one for non-inline,
    // see 2regression/gnu-inline.c.
    NeedDefinition = !WasDefinition && FD && FD->isThisDeclarationADefinition();
  } else if (const VarDecl *VD = dyn_cast<VarDecl>(D)) {
    VD = nonUser(VD);
    Replacement = VD;
    // Replacement may be definition, such as multi/header-var.
    NeedDefinition = VD && (VD->isThisDeclarationADefinition() ==
                            VarDecl::DefinitionKind::Definition);
    if (VD)
      if (MemberSpecializationInfo *MSI = VD->getMemberSpecializationInfo()) {
        TemplateSpecializationKind Kind = MSI->getTemplateSpecializationKind();
        if (Kind == TSK_ImplicitInstantiation ||
            Kind == TSK_ExplicitSpecialization) {
          MSI->setTemplateSpecializationKind(TSK_ImplicitInstantiation);
          MSI->setPointOfInstantiation(SourceLocation());
        }
      }
  } else {
    Replacement = nullptr;
    NeedDefinition = false;
  }
  {
    SmallVector<GlobalValue *, 8> GVs;
    Builder->getDeclGlobalValues(D, GVs);
    Builder->DeclGlobalValues.erase(D);
    for (GlobalValue *GV : GVs) {
      GV->setDecl(nullptr);
      StringRef Name = GV->getName();
      // When a local copy of __cxa_pure_virtual is provided,
      // keep __cxa_pure_virtual IR declaration which may have
      // been there before.
      if (Name == "__cxa_pure_virtual") {
        GVsToKeepDeclaration.insert(GV);
      } else if (Name.startswith("__cxx_global_var_init") ||
                 isa<GlobalIndirectSymbol>(GV)) {
        insertConstantsToEraseCompletely(GV);
      } else if (isSameKind(Replacement, GV)) {
        Builder->insertDeclGlobalValues(Replacement, GV, false);
        if (!NeedDefinition)
          GVsToKeepDeclaration.insert(GV);
      } else {
        insertConstantsToEraseCompletely(GV);
      }
    }
  }
}

void Unloader::removeNamedDeclFromModule(const NamedDecl *ND) {
  auto I = Builder->GlobalDecls.find(ND);
  if (I == Builder->GlobalDecls.end())
    return;
  auto GDs = I->second;
  for (GlobalDecl GD : GDs) {
    auto &MangledDeclNames = Builder->MangledDeclNames;
    auto J = MangledDeclNames.find(GD);
    if (J != MangledDeclNames.end()) {
      StringRef MangledName = J->second;
      Builder->DeferredDecls.erase(MangledName);
      Builder->Replacements.erase(MangledName);
      // With __unknown_anytype there may be multiple unrelated use of the same
      // mangled name.
      Builder->Manglings[MangledName] = GlobalDecl();
      MangledDeclNames.erase(J);
    }
  }
  Builder->GlobalDecls.erase(I);
}

static void removeFunctionAttrs(Function *F) {
  // See TargetTransformInfoImplBase::areInlineCompatible
  // http://llvm.org/docs/LangRef.html#function-attributes
  static AttrBuilder B;
  if (B.empty()) {
    B.addAttribute(Attribute::ArgMemOnly)
        .addAttribute(Attribute::Convergent)
        .addAttribute(Attribute::InaccessibleMemOnly)
        .addAttribute(Attribute::InaccessibleMemOrArgMemOnly)
        .addAttribute(Attribute::NoAlias)
        .addAttribute(Attribute::NoCapture)
        .addAttribute(Attribute::NonNull)
        .addAttribute(Attribute::None)
        .addAttribute(Attribute::NoReturn)
        .addAttribute(Attribute::NoRecurse)
        .addAttribute(Attribute::NoUnwind)
        .addAttribute(Attribute::ReadNone)
        .addAttribute(Attribute::ReadOnly)
        .addAttribute(Attribute::ReturnsTwice)
        .addAttribute(Attribute::UWTable)
        .addAttribute(Attribute::WriteOnly);
  }
  llvm::AttributeList Attr = F->getAttributes();
  unsigned Index;
  if (Attr.hasAttrSomewhere(Attribute::Returned, &Index) && Index)
    Attr = Attr.removeAttribute(F->getContext(), Index, Attribute::Returned);
  Attr = Attr.removeAttributes(F->getContext(),
                               llvm::AttributeList::FunctionIndex, B);
  F->setAttributes(Attr);
}

void Unloader::eraseDefinition(GlobalValue *GV) {
  using namespace llvm;
  GV->setVisibility(GlobalValue::VisibilityTypes::DefaultVisibility);
  if (GlobalObject *GO = dyn_cast<GlobalObject>(GV))
    GV->getContext().pImpl->GlobalObjectMetadata.erase(GO);
  if (GlobalVariable *GVar = dyn_cast<GlobalVariable>(GV)) {
    if (GVar->hasComdat())
      GVar->setComdat(nullptr);
    if (GVar->isDeclaration())
      return;
    // Don't kill zero initializer.
    if (GVar->hasCommonLinkage() && GVar->getInitializer()->isNullValue())
      return;
    // Declaration can have ExternalLinkage or ExternalWeakLinkage.
    if (GVar->getLinkage() != GlobalVariable::LinkageTypes::ExternalWeakLinkage)
      GVar->setLinkage(GlobalVariable::LinkageTypes::ExternalLinkage);
    // Declaration may not be in comdat.
    GVar->setInitializer(nullptr);
  } else if (Function *F = dyn_cast<Function>(GV)) {
    if (F->hasComdat())
      F->setComdat(nullptr);
    if (!F->isDeclaration())
      F->deleteBody();
    F->invalidateMachineFunction();
    F->resetPassed();
    removeFunctionAttrs(F);
    Builder->FunctionTypeInfos.erase(F);
  } else if (GlobalAlias *GA = dyn_cast<GlobalAlias>(GV)) {
    GA->setAliasee(nullptr);
  } else {
    GlobalIFunc *GIF = cast<GlobalIFunc>(GV);
    GIF->setResolver(nullptr);
  }
}

bool Unloader::insertConstantsToEraseCompletely(llvm::Constant *C) {
  return ConstantsToEraseCompletely.insert(C);
}

static bool isFunctionUnloadOK(StringRef Name) {
  // Unnamed function is created in SanitizerStatReport::finish().
  if (!Name.size())
    return true;
  StringRef StartsWithOK[] = {"_GLOBAL",
                              "__tls",
                              "__dtor_",
                              "__clang",
                              "__cxx_global",
                              "_ZTW",
                              "__Block_byref_object",
                              "asan."};
  for (StringRef StartsWith : StartsWithOK)
    if (Name.startswith(StartsWith))
      return true;
  // MicrosoftMangleContextImpl::mangleDynamicInitializer.
  if (Name.startswith("\x01??__") && Name.endswith("@YAXXZ"))
    return true;
  return false;
}

#if 0
static bool isGVUnloadOK(StringRef Name) {
  // MS ABI creates unnamed arrays of type rtti.CompleteObjectLocator(?).
  if (Name.empty())
    return true;
  StringRef StartsWithOK[] = {"switch.table", "llvm.used", ".ref.tmp"};
  for (StringRef StartsWith : StartsWithOK)
    if (Name.startswith(StartsWith))
      return true;
  if (Name.endswith("@@")) {
    // MicrosoftMangleContextImpl::mangleCXXThrowInfo.
    if (Name.startswith("_TI"))
      return true;
    // MicrosoftMangleContextImpl::mangleCXXCatchableTypeArray.
    if (Name.startswith("_CTA"))
      return true;
  }
  return false;
}
#endif

void Unloader::insertConstantAndUsers(Constant *C) {
  using namespace llvm;
  assert(C);
  if (!VisitedConstants.insert(C).second)
    return;
#ifndef NDEBUG
  ConstantsStack.push_back(C);
#endif
  for (User *U : C->users()) {
    if (Instruction *Ins = dyn_cast<Instruction>(U))
      insertConstantAndUsers(Ins->getParent()->getParent());
    else if (BlockAddress *BA = dyn_cast<BlockAddress>(U))
      insertConstantAndUsers(BA->getFunction());
    else
      insertConstantAndUsers(cast<Constant>(U));
  }

  if (insertConstantsToEraseCompletely(C)) {
    // StringRef What;
    if (Function *F = dyn_cast<Function>(C)) {
      if (!isFunctionUnloadOK(F->getName())) {
        // What = "Function";
        insertCachedDeferredDeclsToEmit(F);
      }
    }
#if 0
    else if (GlobalVariable *GV = dyn_cast<GlobalVariable>(C)) {
      if (!isGVUnloadOK(GV->getName()))
        What = "GlobalVariable";
    }
    if (What.size()) {
      StringRef FileName = SM.getFileEntryForID(SM.getMainFileID())->getName();
      llvm::outs() << "In file '" << FileName << "':\n"
                   << "note: system " << What << " '" << C->getName()
                   << "' was removed.\n";
      unsigned Printed = 0;
      for (auto I = ConstantsStack.rbegin(), E = ConstantsStack.rend();;) {
        // Skip last one which is ourselves.
        ++I;
        if (I == E)
          break;
        if (GlobalValue *GV = dyn_cast<GlobalValue>(*I)) {
          outs() << "note: user of '" << GV->getName() << "'.\n";
          ++Printed;
          if (Printed == 10)
            break;
        }
      }
    }
#endif
  }
#ifndef NDEBUG
  ConstantsStack.pop_back();
#endif
}

void Unloader::insertCachedDeferredDeclsToEmit(llvm::GlobalValue *GV) {
  const Decl *D = reinterpret_cast<const Decl *>(GV->getDecl());
  if (const FunctionDecl *FD = dyn_cast_or_null<FunctionDecl>(D))
    if (FD->isThisDeclarationADefinition())
      if (!UD.Decls.count(FD))
        Builder->ErasedDeferredDeclsToEmit.insert(FD);
}

static bool
findErasedType(const Function &F,
               const SmallPtrSetImpl<const llvm::Type *> &ErasedTypes) {
  //  const llvm::Module *M = F.getParent();
  for (const BasicBlock &BB : F) {
    for (const Instruction &I : BB) {
      llvm::Type *T = nullptr;
      if (auto *GEP = dyn_cast<GetElementPtrInst>(&I))
        T = GEP->getSourceElementType();
      else if (auto *AI = dyn_cast<AllocaInst>(&I))
        T = AI->getAllocatedType();
      if (T && ErasedTypes.count(T))
        return true;
    }
  }
  return false;
}

void Unloader::collectConstants() {
  using namespace llvm;
  M->setAlive();
  for (GlobalObject &GO : M->global_objects()) {
    StringRef Name = GO.getName();
    if (Name.startswith("__asan") || Name.startswith(".omp_offload") ||
        Name.startswith("__tgt_"))
      insertConstantsToEraseCompletely(&GO);
  }
#if 0
  for (GlobalAlias &GA : M->aliases())
    insertConstantsToEraseCompletely(&GA);
  for (GlobalIFunc &GIF : M->ifuncs())
    insertConstantsToEraseCompletely(&GIF);
#endif
  for (StringRef Name :
       {"llvm.global_ctors", "llvm.global_dtors", "llvm.global.annotations",
        "__tls_init", "__tls_guard", "asan.module_ctor", "asan.module_dtor",
        "__llvm_gcov_ctr", "__llvm_gcov_init", "__llvm_gcov_writeout"})
    if (GlobalValue *GV = M->getNamedValue(Name))
      insertConstantsToEraseCompletely(GV);
  for (CodeGen::CodeGenModule::Structor &S : Builder->GlobalCtors)
    if (S.Initializer->getName().startswith("_GLOBAL__sub_I"))
      insertConstantsToEraseCompletely(S.Initializer);
  for (CodeGen::CodeGenModule::Structor &S : Builder->GlobalDtors)
    if (S.Initializer->getName().startswith("_GLOBAL__D"))
      insertConstantsToEraseCompletely(S.Initializer);
  if (SanitizerStatReport *SanStats = Builder->SanStats.get()) {
    insertConstantsToEraseCompletely(SanStats->getModuleStatsGV());
    Builder->SanStats.reset();
  }
  // Inlining and InstCombiner::visitBitCast may create GEP instructions
  // for types we now erased, without the AST knowing.
  for (Function &F : *M) {
    if (ConstantsToEraseCompletely.count(&F) || GVsToKeepDeclaration.count(&F))
      continue;
    if (findErasedType(F, ErasedTypes)) {
      // GVsToKeepDeclaration.insert(&F);
      insertConstantsToEraseCompletely(&F);
      insertCachedDeferredDeclsToEmit(&F);
    }
  }
  M->resetAlive();
  ErasedTypes.clear();

  for (GlobalValue *GV : GVsToKeepDeclaration)
    eraseDefinition(GV);
  // Collect all constants to erase and their users.
  // ConstantsToEraseCompletely will change size in this loop
  // and may relocate so iterators are not safe to use.
  for (unsigned I = 0; I != ConstantsToEraseCompletely.size(); ++I) {
    Constant *C = ConstantsToEraseCompletely[I];
    if (GlobalValue *GV = dyn_cast<GlobalValue>(C))
      eraseDefinition(GV);
    insertConstantAndUsers(C);
  }
  VisitedConstants.clear();
}

void Unloader::eraseConstantandUsers(Constant *C) {
  using namespace llvm;
  if (!VisitedConstants.insert(C).second)
    return;
  while (!C->use_empty()) {
    User *U = *(C->user_begin());
    // Should not find any GlobalVariable or Function here, as their
    // definitions were erased and so they can not be users of anything.
    if (isa<GlobalVariable>(U) || isa<Function>(U)) {
#ifndef NDEBUG
      llvm::errs() << "Constant =\n";
      C->dump();
      llvm::errs() << "User =\n";
      U->dump();
#endif
      assert(0 && "Unexpected GlobalVariable or Function user");
    }
    eraseConstantandUsers(cast<Constant>(U));
  }
  if (GlobalValue *GV = dyn_cast<GlobalValue>(C)) {
    Builder->WeakRefReferences.erase(GV);
    GV->eraseFromParent();
  } else {
    C->destroyConstant();
  }
}

template <typename T>
static void eraseIfInConstantsSet(std::vector<T> &V,
                                  SetVector<Constant *> &ConstantsSet) {
  auto I = std::remove_if(V.begin(), V.end(), [&](T t) {
    if (Constant *C = cast_or_null<Constant>(t))
      return (bool)ConstantsSet.count(C);
    else
      return false;
  });
  V.erase(I, V.end());
}

template <typename T>
static void eraseIfSecondInConstantsSet(T &V,
                                        SetVector<Constant *> &ConstantsSet) {
  auto I =
      std::remove_if(V.begin(), V.end(), [&](typename T::value_type &Pair) {
        if (Constant *C = Pair.second)
          return (bool)ConstantsSet.count(C);
        else
          return false;
      });
  V.erase(I, V.end());
}

static void eraseCtorList(CodeGen::CodeGenModule::CtorList &Tors,
                          SetVector<Constant *> &ConstantsSet) {
  auto I = std::remove_if(Tors.begin(), Tors.end(),
                          [&](CodeGen::CodeGenModule::Structor &S) {
                            return ConstantsSet.count(S.Initializer);
                          });
  Tors.erase(I, Tors.end());
}

void Unloader::eraseBuilder() {
  // Vectors are erased in bulk to avoid n^2 complexity.
  {
    // Aliases
    auto &Aliases = Builder->Aliases;
    auto I = std::remove_if(Aliases.begin(), Aliases.end(), [&](GlobalDecl GD) {
      return UD.Decls.count(GD.getDecl());
    });
    Aliases.erase(I, Aliases.end());
  }
#if 0
  {
    // MangledDeclNames
    Builder->MangledDeclNames.remove_if(
        [&](std::pair<GlobalDecl, StringRef> &Pair) {
          return Pair.second.empty();
        });
  }
#endif
  {
    // DeferredVTables
    auto &DeferredVTables = Builder->DeferredVTables;
    auto I = std::remove_if(
        DeferredVTables.begin(), DeferredVTables.end(),
        [&](const CXXRecordDecl *CXXRD) { return UD.Decls.count(CXXRD); });
    DeferredVTables.erase(I, DeferredVTables.end());
  }
  {
    // OpportunisticVTables cleared in CodeGenModule::EmitVTablesOpportunistically.
  }
  {
    // CXXThreadLocals
    auto &CXXThreadLocals = Builder->CXXThreadLocals;
    auto I =
        std::remove_if(CXXThreadLocals.begin(), CXXThreadLocals.end(),
                       [&](const VarDecl *VD) { return UD.Decls.count(VD); });
    CXXThreadLocals.erase(I, CXXThreadLocals.end());
  }
  {
    // StaticExternCValues
    Builder->StaticExternCValues.remove_if(
        [&](std::pair<IdentifierInfo *, GlobalValue *> Pair) {
          return ConstantsToEraseCompletely.count(Pair.second);
        });
  }
  eraseCtorList(Builder->GlobalCtors, ConstantsToEraseCompletely);
  eraseCtorList(Builder->GlobalDtors, ConstantsToEraseCompletely);
  eraseIfInConstantsSet(Builder->CXXThreadLocalInits,
                        ConstantsToEraseCompletely);
  eraseIfInConstantsSet(Builder->CXXGlobalInits, ConstantsToEraseCompletely);
  eraseIfInConstantsSet(Builder->LLVMUsed, ConstantsToEraseCompletely);
  eraseIfInConstantsSet(Builder->LLVMCompilerUsed, ConstantsToEraseCompletely);
  eraseIfInConstantsSet(Builder->Annotations, ConstantsToEraseCompletely);
  // ConstantStringMap
  eraseDenseMap(Builder->ConstantStringMap,
                [&](std::pair<llvm::Constant *, llvm::GlobalVariable *> Pair) {
                  return ConstantsToEraseCompletely.count(Pair.first) ||
                         ConstantsToEraseCompletely.count(Pair.second);
                });
  eraseIfSecondInConstantsSet(Builder->PrioritizedCXXGlobalInits,
                              ConstantsToEraseCompletely);
  eraseIfSecondInConstantsSet(Builder->CXXGlobalDtors,
                              ConstantsToEraseCompletely);
  Builder->GlobalValReplacements.clear();
  // Could not templatize these due to StringMap::key_type & first() vs first.
  {
    // Replacements
    auto &Replacements = Builder->Replacements;
    typedef CodeGen::CodeGenModule::ReplacementsTy::iterator Iterator;
    for (Iterator I = Replacements.begin(), E = Replacements.end(); I != E;) {
      auto J = I;
      ++J;
      if (ConstantsToEraseCompletely.count(I->getValue()))
        Replacements.erase(I);
      I = J;
    }
  }
  eraseDenseMap(Builder->MaterializedGlobalTemporaryMap,
                [&](std::pair<const clang::Expr *, llvm::Constant *> Pair) {
                  return ConstantsToEraseCompletely.count(Pair.second);
                });
  auto shouldErase = [&](llvm::Value *V) {
    if (Constant *C = dyn_cast_or_null<Constant>(V))
      return ConstantsToEraseCompletely.count(C);
    return (size_t)1;
  };
  eraseDenseMap(
      Builder->FunctionTypeInfos,
      [&](std::pair<llvm::Function *,
                    llvm::SmallVector<llvm::WeakTrackingVH, 4>> &Pair) {
        return ConstantsToEraseCompletely.count(Pair.first) ||
               llvm::any_of(Pair.second, shouldErase);
      });
  // LLVMContextImpl tables are erased in Function::dropAllReferences().
  if (CodeGen::CGOpenMPRuntime *OpenMPRuntime = Builder->OpenMPRuntime.get())
    eraseOpenMP(OpenMPRuntime);
}

void Unloader::eraseType(const Type *T, bool EraseDeclaration) {
  CodeGen::CodeGenTypes &Types = Builder->getTypes();
  CodeGen::CodeGenTBAA *TBAA = Builder->TBAA.get();
  if (T->isRecordType()) {
    if (TBAA) {
      TBAA->MetadataCache.erase(T);
      TBAA->BaseTypeMetadataCache.erase(T);
      TBAA->StructMetadataCache.erase(T);
    }
    // Erase LLVM StructType definition.
    auto I = Types.RecordDeclTypes.find(T);
    if (I != Types.RecordDeclTypes.end()) {
      llvm::StructType *ST = I->second;
      ErasedTypes.insert(ST);
      M->getDataLayout().erase(ST);
      ST->resetBody();
      if (EraseDeclaration) {
        llvm::SmallString<256> BaseName(ST->getName());
        // Possibly created in CodeGenTypes::ComputeRecordLayout.
        BaseName.append(".base");
        if (llvm::StructType *base = M->getTypeByName(BaseName))
          base->setName("");
        ST->setName("");
        Types.RecordDeclTypes.erase(I);
      }
    }
    Types.CGRecordLayouts.erase(T);
    Types.RecordsBeingLaidOut.erase(T);
  }
  if (EraseDeclaration) {
    Types.TypeCache.erase(T);
    if (const RecordType *RT = dyn_cast<RecordType>(T))
      S.getASTContext().NeedDestructor.erase(RT);
  }
}

void Unloader::eraseCodeGenTypes() {
  for (const Type *T : UD.Types)
    eraseType(T, true);
  for (const Type *T : TypesToKeepDeclaration)
    eraseType(T, false);
}

static void resetLinkage(CodeGen::CodeGenModule *Builder, NamedDecl *ND) {
  auto I = Builder->GlobalDecls.find(ND);
  if (I == Builder->GlobalDecls.end())
    return;
  llvm::Module *M = &Builder->getModule();
  auto GDs = I->second;
  for (GlobalDecl GD : GDs) {
    auto &MangledDeclNames = Builder->MangledDeclNames;
    auto J = MangledDeclNames.find(GD);
    if (J != MangledDeclNames.end()) {
      StringRef MangledName = J->second;
      GlobalValue *GV = M->getNamedValue(MangledName);
      if (!GV)
        continue;
      if (Function *F = dyn_cast<Function>(GV)) {
        Builder->setFunctionLinkage(GD, F);
        if (F->isDeclarationForLinker())
          F->setComdat(nullptr);
      } else if (GlobalAlias *GA = dyn_cast<GlobalAlias>(GV)) {
        GA->setLinkage(GlobalValue::LinkageTypes::LinkOnceODRLinkage);
      }
    }
  }
}

void Unloader::restoreWeakRef() {}

void Unloader::processInstantiationsChange() {
  auto &InstantiationChanges = C.InstantiationChanges;
  for (auto Change : InstantiationChanges) {
    NamedDecl *ND = const_cast<NamedDecl *>(Change.first);
    FileID FID = SM.getFileID(SM.getExpansionLoc(Change.second.NewLoc));
    const FileEntry *FE = SM.getFileEntryForID(FID);
    if (UD.FileEntries.count(FE)) {
      TemplateSpecializationKind Old_TSK = Change.second.Old_TSK;
      if (auto *CTSD = dyn_cast<ClassTemplateSpecializationDecl>(ND)) {
        if (Old_TSK != TSK_ImplicitInstantiation)
          CTSD->setTemplateSpecializationKind(Old_TSK);
        CTSD->setLocation(Change.second.OldLoc);
      } else {
        cast<FunctionDecl>(ND)->setTemplateSpecializationKind(Old_TSK);
        if (Builder)
          resetLinkage(Builder, ND);
      }
    }
  }
  C.InstantiationChanges.clear();
}

void Unloader::eraseFileEntries() {
  // Finally, really remove all dead FileEntries.
  FileManager &FM = PP.getFileManager();
  HeaderSearch &HS = PP.getHeaderSearchInfo();
  PP.VisibleFEs.remove_if(
      [&](const FileEntry *FE) { return UD.FileEntries.count(FE); });
  CodeGen::CGDebugInfo *CGDI;
  if (Builder)
    CGDI = Builder->getModuleDebugInfo();
  else
    CGDI = nullptr;
  for (const FileEntry *FE : UD.FileEntries) {
    assert(FE);
    PP.IncludeMgr.erase(FE);
    PP.FilesIdentifiersValues.erase(FE);
    FDMapping.erase(FE);
    S.CachedPendingInstantiations.erase(FE);
    FE->closeFile();
    if (Builder)
      if (CGDI)
        CGDI->DIFileCache.erase(FE->getName());
    // Reset file info such as #pragma once.
    HeaderFileInfo &HFI = HS.getFileInfo(FE);
    if (const IdentifierInfo *II = HFI.ControllingMacro)
      PP.IncludeGuards.erase(II);
    HFI = HeaderFileInfo();
    SM.eraseFE(FE)->OrigEntry = nullptr;
  }
  FM.invalidateCache(UD.FileEntries);
  HS.clear();
}

void Unloader::unload() {
  S.VisibleNamespaceCache.clear();
  if (Builder) {
    M = &Builder->getModule();
    SmallPtrSet<const CXXRecordDecl *, 16> UserCXXRecords;
    for (const Decl *D : UD.Decls)
      if (const CXXRecordDecl *CXXRD = dyn_cast<CXXRecordDecl>(D))
        UserCXXRecords.insert(CXXRD);
    Builder->ABI->eraseUserDecls(UserCXXRecords);
    auto &TypeGlobalValues = Builder->TypeGlobalValues;
    for (const Type *T : UD.Types) {
      auto I = TypeGlobalValues.find(T);
      if (I != TypeGlobalValues.end()) {
        for (Value *V : I->second)
          if (llvm::GlobalValue *GV = GlobalValue::fromValue(V))
            insertConstantsToEraseCompletely(GV);
        TypeGlobalValues.erase(I);
      }
    }
  }
  // Namespaces are required for proper unloading of rest.
  for (const Decl *D : UD.Decls)
    if (!isa<NamespaceDecl>(D))
      UnloadDecl(const_cast<Decl *>(D));
  for (const Decl *D : UD.Decls)
    if (isa<NamespaceDecl>(D))
      UnloadDecl(const_cast<Decl *>(D));
  unloadVectors();
  unloadDeclContexts();
#ifndef NDEBUG
  for (const Decl *D : UD.Decls)
    if (auto *ND = dyn_cast<NamedDecl>(D))
      memset((void *)const_cast<NamedDecl *>(ND), 0, sizeof(*ND));
#endif
  eraseDeclsASTContext();
  eraseTypesASTContext();
  eraseNestedNameSpecifiersASTContext();
  // Must be after Decls are processed.
  recanonTypes();
  processInstantiationsChange();
  if (Builder) {
    eraseDenseMap(Builder->getTypes().FunctionInfos,
                  [&](CodeGen::CGFunctionInfo &FI) {
                    for (CodeGen::CGFunctionInfoArgInfo &AI : FI.arguments()) {
                      const Type *T = AI.type.getTypePtr();
                      if (UD.Types.count(T) || TypesToKeepDeclaration.count(T))
                        return true;
                    }
                    return false;
                  });
    eraseCodeGenTypes();
    collectConstants();
    eraseBuilder();
    Builder->ABI->eraseUserConstants(ConstantsToEraseCompletely);
    // Definitions was erased earlier due to circular dependencies.
    for (Constant *C : ConstantsToEraseCompletely)
      eraseConstantandUsers(C);
    eraseMetadata();
    if (CodeGen::CGDebugInfo *CGDI = Builder->getModuleDebugInfo())
      eraseDebugTypes(CGDI);
    restoreWeakRef();
    eraseDenseMap(Builder->StaticLocalDeclMap,
                  [&](std::pair<const Decl *, llvm::Constant *> &I) {
                    return Visited.count(const_cast<Decl *>(I.first)) ||
                           ConstantsToEraseCompletely.count(I.second);
                  });
    eraseDenseMap(Builder->DelayedCXXInitPosition,
                  [&](std::pair<const Decl *, unsigned> &I) {
                    return Visited.count(const_cast<Decl *>(I.first));
                  });
  }
  eraseMacros();
  eraseFileEntries();
}

} // namespace clang
