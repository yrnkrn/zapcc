// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "DependencyMap.h"
#include "UnloadSelector.h"
#include "UserReplacer.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Path.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/StmtCXX.h"
#include "clang/Lex/Preprocessor.h"

#define DEBUG_TYPE "zapcc-files"

namespace clang {

DependencyMap::~DependencyMap() {}

static bool wasModified(FileManager &FM, const FileEntry *FE) {
  time_t T1 = FE->getModificationTime();
  llvm::sys::fs::file_status Result;
  if (llvm::sys::fs::status(FE->getName(), Result))
    return false;
  time_t T2 = llvm::sys::toTimeT(Result.getLastModificationTime());
  return T1 != T2;
}

void DependencyMap::findModifiedFiles(FileManager &FM) {
  llvm::SmallVector<const FileEntry *,32> FEs;
  for (auto &Pair : FM.UniqueRealFiles)
    FEs.push_back(&Pair.second);
  std::sort(FEs.begin(), FEs.end(),
            [](const FileEntry *FE1, const FileEntry *FE2) {
              return FE1->getModificationTime() > FE2->getModificationTime();
            });
  const unsigned MaxFiles = 100;
  if (FEs.size() > MaxFiles)
    FEs.set_size(MaxFiles);
  for (const FileEntry *FE : FEs)
    if (wasModified(FM, FE))
      UD.FileEntries.insert(FE);
}

void DependencyMap::update(ASTContext *Ctx, Preprocessor *Pre) {
  Context = Ctx;
  PP = Pre;
  SourceManager &SM = Context->getSourceManager();
  const FileEntry *MainFile = SM.getFileEntryForID(SM.getMainFileID());
  for (auto I : PP->NewFEs) {
    FileEntry *Includee = const_cast<FileEntry *>(I.first);
    FileEntry *Includer = const_cast<FileEntry *>(I.second);
    Visited.erase(Includee);
    StringRef Name = Includee->getName();
    assert(Name.size());
    StringRef Ext = llvm::sys::path::extension(Name);
    if (Includee == MainFile ||
        (Includer == MainFile && Ext.startswith(".c")) || US->isMutable(Name))
      if (UD.FileEntries.insert(Includee))
        ++UD.MutableFileEntries;
    if (Includer)
      // Includer included Includee and thus depends on Includee.
      if (Includee != Includer)
        FileEntryDependees[Includer].insert(Includee);
  }
  // Delete modified decls dependenices and insert into NewDecls to reprocess.
  for (auto Pair : Context->ModifiedDecls) {
    Decl *Dependee = Pair.first;
    Decl *Dependent = Pair.second;
    Context->NewDecls.push_back(Dependent);
    if (!Dependee)
      continue;
    auto I = Dependents.find(DependentType(Dependee).getOpaqueValue());
    if (I != Dependents.end())
      I->second.erase(Dependent);
    auto J = Dependents.find(DependentType(Dependent).getOpaqueValue());
    if (J != Dependents.end())
      J->second.erase(Dependee);
  }
  Context->ModifiedDecls.clear();
  // Update Decl-File maps.
  for (Decl *D : Context->NewDecls) {
    auto FileIDEntry = SM.getFileIDEntry(D->getLocStart());
    if (const FileEntry *FE = FileIDEntry.second)
      FDMapping.insert(D, FE);
    Visited.erase(D);
  }
  // Now, update dependencies.
  for (auto I : PP->NewFEs) {
    FileEntry *DependFE = const_cast<FileEntry *>(I.first);
    // MainFile will be always removed, we do not need its dependees.
    if (DependFE != MainFile)
      findInFileEntry(DependFE);
  }
  PP->NewFEs.clear();
  for (Decl *D : Context->NewDecls)
    findInDecl(D);
  Context->NewDecls.clear();
  for (Type *T : Context->NewTypes)
    findInType(T);
  Context->NewTypes.clear();
  for (NestedNameSpecifier *NNS : Context->NewNestedNameSpecifiers)
    findInNestedNameSpecifier(NNS);
  Context->NewNestedNameSpecifiers.clear();
}

void DependencyMap::collectUsers() {
  VisitedUsers.clear();
  // Collect Dependencies and Decls of User FileEntries, allowing the SetVector
  // to grow while collecting.
  for (unsigned I = 0; I != UD.FileEntries.size(); ++I) {
    const FileEntry *FE = UD.FileEntries[I];
    collectUsersOf(const_cast<FileEntry *>(FE));
    auto J = FDMapping.find(FE);
    if (J != FDMapping.file_end())
      for (const Decl *D : J->second)
        collectUsersOf(const_cast<Decl *>(D));
  }
  // Now remove all user* and dependencies from graph.
  RemoveDependencies = true;
  for (const Decl *D : UD.Decls) {
    findInDecl(D);
    Visited.erase(const_cast<Decl *>(D));
  }
  for (Type *T : UD.Types) {
    findInType(T);
    Visited.erase(T);
    UD.RecanonTypes.erase(T);
  }
  for (NestedNameSpecifier *NNS : UD.NestedNameSpecifiers) {
    findInNestedNameSpecifier(NNS);
    Visited.erase(NNS);
  }
  for (const FileEntry *FE : UD.FileEntries) {
    findInFileEntry(FE);
    Visited.erase(const_cast<FileEntry *>(FE));
    FileEntryDependees.erase(FE);
  }
  RemoveDependencies = false;
#if 0
    for (auto I : Dependents)
      for (DependentType DT : I.second) {
        if (Decl *D = DT.dyn_cast<Decl *>())
          assert(!UD.Decls.count(D));
        else if (Type *T = DT.dyn_cast<Type *>())
          assert(!UD.Types.count(T));
        else if (NestedNameSpecifier *NNS = DT.dyn_cast<NestedNameSpecifier *>())
          assert(!UD.NestedNameSpecifiers.count(NNS));
        else {
          FileEntry *FE = DT.get<FileEntry *>();
          assert(!UD.FileEntries.count(FE));
        }
      }
#endif
}

static Decl *findReplacement(Decl *Dependee,
                             llvm::SetVector<const Decl *> &UserDecls);

static TemplateTypeParmDecl *
findReplacementTemplateTypeParm(TemplateTypeParmDecl *TTPD,
                                llvm::SetVector<const Decl *> &UserDecls) {
  // TemplateTypeParmDecl don't have a previous Decl,
  // we have to go through the container.
  DeclContext *DC = TTPD->getDeclContext();
  CXXRecordDecl *CXXRD = dyn_cast_or_null<CXXRecordDecl>(DC);
  if (!CXXRD)
    return nullptr;
  TemplateDecl *Old = CXXRD->getDescribedClassTemplate();
  TemplateDecl *New =
      cast_or_null<TemplateDecl>(findReplacement(Old, UserDecls));
  if (!New)
    return nullptr;
  TemplateParameterList *OldTPL = Old->getTemplateParameters();
  TemplateParameterList *NewTPL = New->getTemplateParameters();
  ArrayRef<NamedDecl *> OldParams = OldTPL->asArray();
  ArrayRef<NamedDecl *> NewParams = NewTPL->asArray();
  assert(OldParams.size() == NewParams.size());
  for (unsigned i = 0, e = OldParams.size(); i < e; ++i)
    if (OldParams[i] == TTPD)
      return cast<TemplateTypeParmDecl>(NewParams[i]);
  return nullptr;
}

static ParmVarDecl *
findReplacementParmVarDecl(ParmVarDecl *PVD,
                           llvm::SetVector<const Decl *> &UserDecls) {
  if (!PVD->getInit())
    return nullptr;
  DeclContext *DC = PVD->getDeclContext();
  FunctionDecl *Old = dyn_cast_or_null<FunctionDecl>(DC);
  if (!Old)
    return nullptr;
  FunctionDecl *New =
      cast_or_null<FunctionDecl>(findReplacement(Old, UserDecls));
  if (!New)
    return nullptr;
  unsigned OldNumParams = Old->getNumParams();
  assert(OldNumParams == New->getNumParams());
  for (unsigned i = 0; i < OldNumParams; ++i)
    if (Old->getParamDecl(i) == PVD)
      return (New->getParamDecl(i));
  return nullptr;
}

static Decl *findReplacement(Decl *Dependee,
                             llvm::SetVector<const Decl *> &UserDecls) {
  if (!Dependee)
    return nullptr;
  if (TemplateTypeParmDecl *TTPD = dyn_cast<TemplateTypeParmDecl>(Dependee))
    return findReplacementTemplateTypeParm(TTPD, UserDecls);
  else if (ParmVarDecl *PVD = dyn_cast<ParmVarDecl>(Dependee))
    return findReplacementParmVarDecl(PVD, UserDecls);
  Decl *Replacement = nullptr;
  Decl *R = Dependee->getMostRecentDecl();
  while (R) {
    if (R != Dependee && R->getKind() == Dependee->getKind() &&
        !UserDecls.count(R)) {
      R->setIsUsed();
      Replacement = R;
    }
    R = R->getPreviousDecl();
  }
  return Replacement;
}

static bool shouldRecanonType(Type *T) {
  if (!T->isDependentType())
    return false;
  if (DecltypeType *DT = dyn_cast<DecltypeType>(T))
    if (DT->getUnderlyingExpr()->isInstantiationDependent())
      return true;
  return isa<TemplateSpecializationType>(T) || isa<FunctionProtoType>(T) ||
         isa<DependentNameType>(T);
}

void DependencyMap::collectUsersOf(DependentType DT) {
  if (!VisitedUsers.insert(DT).second)
    return;
#ifndef NDEBUG
  DependentStack.push_back(DT);
  DEBUG(if (DT.is<FileEntry *>()) {
    for (auto I = DependentStack.rbegin(), E = DependentStack.rend(); I != E;
         ++I) {
      if (I == DependentStack.rbegin())
        puts("\nDEPENDENTS ================================== BEGIN");
      else
        puts("           ==================================");
      print(*I);
    }
    puts("DEPENDENTS ================================== END\n");
  });
#endif
  Decl *DependeeD = DT.dyn_cast<Decl *>();
  Type *DependeeT = DT.dyn_cast<Type *>();
  if (DependeeD)
    UD.Decls.insert(DependeeD);
  else if (DependeeT)
    UD.Types.insert(DependeeT);
  else if (NestedNameSpecifier *NNS = DT.dyn_cast<NestedNameSpecifier *>())
    UD.NestedNameSpecifiers.insert(NNS);
  else {
    FileEntry *FE = DT.get<FileEntry *>();
    assert(FE->getName().size());
    UD.FileEntries.insert(FE);
  }
  auto I = Dependents.find(DT.getOpaqueValue());
  if (I != Dependents.end()) {
    // Steal the set to avoid circular loops.
    decltype(I->second) DependentsSet = std::move(I->second);
    I->second.clear();
    Dependents.erase(I);
    Decl *Replacement = DependeeD;
    for (DependentType Dependent : DependentsSet) {
      Type *DependentT = Dependent.dyn_cast<Type *>();
      if (Replacement && UD.Decls.count(Replacement))
        Replacement = findReplacement(DependeeD, UD.Decls);
      if (Replacement) {
        UserReplacer UR(DependeeD, Replacement);
        if (Decl *D = Dependent.dyn_cast<Decl *>())
          UR.replaceDecl(D);
        else if (DependentT)
          UR.replaceType(DependentT);
        else if (NestedNameSpecifier *NNS =
                     Dependent.dyn_cast<NestedNameSpecifier *>())
          UR.replaceNestedNameSpecifier(NNS);
        // Update Dependents to replacement dependee.
        if (UR.wasReplaced()) {
          DependentType NewDependee(Replacement);
          Dependents[NewDependee.getOpaqueValue()].insert(Dependent);
          continue;
        }
      } else if (DependeeT && DependentT) {
        if (DependeeT == DependentT->getCanonicalTypeInternal().getTypePtr() &&
            !UD.Types.count(DependentT) && shouldRecanonType(DependentT)) {
          UD.RecanonTypes.insert(DependentT);
          continue;
        }
      }
      collectUsersOf(Dependent);
    }
  }
#ifndef NDEBUG
  DependentStack.pop_back();
#endif
}

bool DependencyMap::dependsOnEmittedFunction(const VarDecl *VD) {
  VisitedDepend.clear();
  auto I = Dependents.find(const_cast<VarDecl *>(VD));
  if (I != Dependents.end())
    for (DependentType Dependent : I->second)
      if (Decl *Dep = Dependent.dyn_cast<Decl *>())
        if (FunctionDecl *DepFD = dyn_cast<FunctionDecl>(Dep))
          if (dependsOnEmittedFunctionOf(DepFD))
            return true;
  return false;
}

bool DependencyMap::dependsOnEmittedFunctionOf(FunctionDecl *FD) {
  if (!VisitedDepend.insert(FD).second)
    return false;
  if (!Context->LiveDecls.count(FD))
    return false;
  if (FD->getLexicalDeclContext()->isFileContext()) {
    TemplateSpecializationKind Kind = FD->getTemplateSpecializationKind();
    if (Kind == TSK_Undeclared || Kind == TSK_ExplicitSpecialization)
      return true;
  } else if (const CXXMethodDecl *MD = dyn_cast<CXXMethodDecl>(FD)) {
    if (MD->isVirtual()) {
      const CXXRecordDecl *RD = MD->getParent();
      assert(RD->isDynamicClass());
      for (CXXMethodDecl *Method : RD->methods())
        if (Method != MD)
          if (dependsOnEmittedFunctionOf(Method))
            return true;
    }
  }
  auto I = Dependents.find(FD);
  if (I != Dependents.end())
    for (DependentType Dependent : I->second)
      if (Decl *Dep = Dependent.dyn_cast<Decl *>())
        if (isa<FunctionDecl>(Dep))
          if (FunctionDecl *DepFD = dyn_cast<FunctionDecl>(Dep))
            if (dependsOnEmittedFunctionOf(DepFD))
              return true;
  return false;
}

void DependencyMap::print(DependentType DT) {
  if (Decl *D = DT.dyn_cast<Decl *>()) {
    D->dump();
    llvm::errs() << "\n";
  } else if (Type *T = DT.dyn_cast<Type *>()) {
    T->dump();
    llvm::errs() << "\n";
  } else if (NestedNameSpecifier *NNS = DT.dyn_cast<NestedNameSpecifier *>()) {
    NNS->dump();
    llvm::errs() << "\n";
  } else {
    FileEntry *FE = DT.get<FileEntry *>();
    llvm::errs() << FE << " " << FE->getName() << "\n";
  }
}

bool DependencyMap::findProlog(DependentType NewDependee,
                               DependentType &LastDependent) {
  if (RemoveDependencies) {
    if (CurrentDependent.getOpaqueValue()) {
      auto I = Dependents.find(NewDependee.getOpaqueValue());
      if (I != Dependents.end())
        I->second.erase(CurrentDependent);
      return true;
    }
  } else {
    if (!CurrentDependent.isNull())
      Dependents[NewDependee.getOpaqueValue()].insert(CurrentDependent);
    if (!Visited.insert(NewDependee).second)
      return true;
  }
  LastDependent = CurrentDependent;
  CurrentDependent = NewDependee;
  return false;
}

static bool isBuiltinDependent(const Type *T) {
  while (T->isAnyPointerType())
    T = T->getPointeeType().getTypePtr();
  return isa<BuiltinType>(T);
}

void DependencyMap::findInType(const Type *T) {
  findInType(const_cast<Type *>(T));
}

void DependencyMap::findInType(Type *T) {
  if (!T)
    return;
  if (isBuiltinDependent(T))
    return;
  DependentType LastDependent;
  if (findProlog(T, LastDependent))
    return;

  //  if (!isa<TemplateTypeParmType>(T) && !isa<FunctionProtoType>(T)) {
  const Type *Canon = T->getCanonicalTypeInternal().getTypePtr();
  if (Canon != T)
    findInType(Canon);
  //  }

  switch (T->getTypeClass()) {
  case Type::TypeClass::Builtin: {
    break;
  }
  case Type::TypeClass::Complex: {
    ComplexType *CT = cast<ComplexType>(T);
    findInQualType(CT->getElementType());
    break;
  }
  case Type::TypeClass::Paren: {
    ParenType *PT = cast<ParenType>(T);
    findInQualType(PT->getInnerType());
    break;
  }
  case Type::TypeClass::Adjusted: {
    AdjustedType *AT = cast<AdjustedType>(T);
    findInQualType(AT->getOriginalType());
    findInQualType(AT->getAdjustedType());
    break;
  }
  case Type::TypeClass::Decayed: {
    DecayedType *DT = cast<DecayedType>(T);
    findInQualType(DT->getOriginalType());
    findInQualType(DT->getDecayedType());
    findInQualType(DT->getPointeeType());
    break;
  }
  case Type::TypeClass::Pointer: {
    PointerType *PT = cast<PointerType>(T);
    findInQualType(PT->getPointeeType());
    break;
  }
  case Type::TypeClass::BlockPointer: {
    BlockPointerType *BPT = cast<BlockPointerType>(T);
    findInQualType(BPT->getPointeeType());
    break;
  }
  case Type::TypeClass::LValueReference:
  case Type::TypeClass::RValueReference: {
    ReferenceType *RT = cast<ReferenceType>(T);
    findInQualType(RT->getPointeeType());
    break;
  }
  case Type::TypeClass::MemberPointer: {
    MemberPointerType *MPT = cast<MemberPointerType>(T);
    findInQualType(MPT->getPointeeType());
    findInType(MPT->getClass());
    break;
  }
  case Type::TypeClass::ConstantArray:
  case Type::TypeClass::IncompleteArray: {
    ArrayType *AT = cast<ArrayType>(T);
    findInQualType(AT->getElementType());
    break;
  }
  case Type::TypeClass::VariableArray: {
    VariableArrayType *VAT = cast<VariableArrayType>(T);
    findInStmt(VAT->getSizeExpr());
    findInQualType(VAT->getElementType());
    break;
  }
  case Type::TypeClass::DependentSizedArray: {
    DependentSizedArrayType *DSAT = cast<DependentSizedArrayType>(T);
    findInStmt(DSAT->getSizeExpr());
    findInQualType(DSAT->getElementType());
    break;
  }
  case Type::TypeClass::DependentSizedExtVector: {
    DependentSizedExtVectorType *DSEVT = cast<DependentSizedExtVectorType>(T);
    findInStmt(DSEVT->getSizeExpr());
    findInQualType(DSEVT->getElementType());
    break;
  }
  case Type::TypeClass::Vector:
  case Type::TypeClass::ExtVector: {
    VectorType *VT = cast<VectorType>(T);
    findInQualType(VT->getElementType());
    break;
  }
  case Type::TypeClass::FunctionProto: {
    FunctionProtoType *FPT = cast<FunctionProtoType>(T);
    findInQualType(FPT->getReturnType());
    for (QualType QT : FPT->param_types())
      findInQualType(QT);
    for (QualType QT : FPT->exceptions())
      findInQualType(QT);
    FunctionProtoType::ExtProtoInfo EPI = FPT->getExtProtoInfo();
    for (QualType QT : EPI.ExceptionSpec.Exceptions)
      findInQualType(QT);
    findInStmt(EPI.ExceptionSpec.NoexceptExpr);
    findInDecl(EPI.ExceptionSpec.SourceDecl);
    findInDecl(EPI.ExceptionSpec.SourceTemplate);
    break;
  }
  case Type::TypeClass::FunctionNoProto: {
    FunctionNoProtoType *FNPT = cast<FunctionNoProtoType>(T);
    findInQualType(FNPT->getReturnType());
    break;
  }
  case Type::TypeClass::UnresolvedUsing: {
    UnresolvedUsingType *UUT = cast<UnresolvedUsingType>(T);
    findInDecl(UUT->getDecl());
    break;
  }
  case Type::TypeClass::Typedef: {
    TypedefType *TT = cast<TypedefType>(T);
    findInDecl(TT->getDecl());
    break;
  }
  case Type::TypeClass::TypeOfExpr: {
    TypeOfExprType *TOET = cast<TypeOfExprType>(T);
    findInStmt(TOET->getUnderlyingExpr());
    break;
  }
  case Type::TypeClass::TypeOf: {
    TypeOfType *TOT = cast<TypeOfType>(T);
    findInQualType(TOT->getUnderlyingType());
    break;
  }
  case Type::TypeClass::Decltype: {
    DecltypeType *DT = cast<DecltypeType>(T);
    findInStmt(DT->getUnderlyingExpr());
    if (DT->isSugared())
      findInQualType(DT->getUnderlyingType());
    break;
  }
  case Type::TypeClass::UnaryTransform: {
    UnaryTransformType *UTT = cast<UnaryTransformType>(T);
    findInQualType(UTT->getUnderlyingType());
    findInQualType(UTT->getBaseType());
    break;
  }
  case Type::TypeClass::Enum:
  case Type::TypeClass::Record: {
    TagType *TT = cast<TagType>(T);
    findInDecl(TT->getRawDecl());
    break;
  }
  case Type::TypeClass::Attributed: {
    AttributedType *AT = cast<AttributedType>(T);
    findInQualType(AT->getModifiedType());
    findInQualType(AT->getEquivalentType());
    break;
  }
  case Type::TypeClass::TemplateTypeParm: {
    TemplateTypeParmType *TTPT = cast<TemplateTypeParmType>(T);
    if (!TTPT->isCanonicalUnqualified())
      findInDecl(TTPT->getDecl());
    break;
  }
  case Type::TypeClass::SubstTemplateTypeParm: {
    SubstTemplateTypeParmType *STTPT = cast<SubstTemplateTypeParmType>(T);
    findInQualType(STTPT->getReplacementType());
    const TemplateTypeParmType *TTPT = STTPT->getReplacedParameter();
    findInType(TTPT);
    break;
  }
  case Type::TypeClass::SubstTemplateTypeParmPack: {
    const SubstTemplateTypeParmPackType *STTPPT =
        cast<SubstTemplateTypeParmPackType>(T);
    const TemplateTypeParmType *TTPT = STTPPT->getReplacedParameter();
    findInType(TTPT);
    TemplateArgument TA = STTPPT->getArgumentPack();
    findInTemplateArgument(&TA);
    break;
  }
  case Type::TypeClass::Auto: {
    AutoType *AT = cast<AutoType>(T);
    if (AT->isSugared())
      findInQualType(AT->desugar());
    break;
  }
  case Type::TypeClass::DeducedTemplateSpecialization: {
    auto *DTST = cast<DeducedTemplateSpecializationType>(T);
    findInTemplateName(DTST->getTemplateName());
    break;
  }
  case Type::TypeClass::TemplateSpecialization: {
    TemplateSpecializationType *TST = cast<TemplateSpecializationType>(T);
    findInTemplateName(TST->getTemplateName());
    if (TST->isTypeAlias())
      findInQualType(TST->getAliasedType());
    for (const TemplateArgument &TA : *TST)
      findInTemplateArgument(&TA);
    break;
  }
  case Type::TypeClass::InjectedClassName: {
    InjectedClassNameType *ICNT = cast<InjectedClassNameType>(T);
    CXXRecordDecl *CXXRD = ICNT->getRawDecl();
    findInDecl(CXXRD);
    findInQualType(ICNT->getInjectedSpecializationType());
    break;
  }
  case Type::TypeClass::Elaborated: {
    ElaboratedType *ET = cast<ElaboratedType>(T);
    findInQualType(ET->getNamedType());
    findInNestedNameSpecifier(ET->getQualifier());
    break;
  }
  case Type::TypeClass::DependentName: {
    DependentNameType *DNT = cast<DependentNameType>(T);
    findInNestedNameSpecifier(DNT->getQualifier());
    break;
  }
  case Type::TypeClass::DependentTemplateSpecialization: {
    DependentTemplateSpecializationType *DTST =
        cast<DependentTemplateSpecializationType>(T);
    for (const TemplateArgument &TA : *DTST)
      findInTemplateArgument(&TA);
    findInNestedNameSpecifier(DTST->getQualifier());
    break;
  }
  case Type::TypeClass::PackExpansion: {
    PackExpansionType *PET = cast<PackExpansionType>(T);
    findInQualType(PET->getPattern());
    break;
  }
  case Type::TypeClass::ObjCObject: {
    ObjCObjectType *OCOT = cast<ObjCObjectType>(T);
    findInQualType(OCOT->getBaseType());
    break;
  }
  case Type::TypeClass::ObjCInterface: {
    ObjCInterfaceType *OCI = cast<ObjCInterfaceType>(T);
    findInQualType(OCI->getBaseType());
    findInDecl(OCI->getDecl());
    break;
  }
  case Type::TypeClass::ObjCObjectPointer: {
    ObjCObjectPointerType *OPT = cast<ObjCObjectPointerType>(T);
    findInQualType(OPT->getPointeeType());
    break;
  }
  case Type::TypeClass::ObjCTypeParam: {
    ObjCTypeParamType *OPT = cast<ObjCTypeParamType>(T);
    findInDecl(OPT->getDecl());
    break;
  }
  case Type::TypeClass::Atomic: {
    AtomicType *AT = cast<AtomicType>(T);
    findInQualType(AT->getValueType());
    break;
  }
  case Type::TypeClass::Pipe: {
    PipeType *PT = cast<PipeType>(T);
    findInQualType(PT->getElementType());
    break;
  }
  } // switch
  CurrentDependent = LastDependent;
}

void DependencyMap::findInQualType(const QualType QT) {
  if (QT.isNull())
    return;
  findInType(QT.getTypePtr());
} // DependencyMap::findInQualType

void DependencyMap::findInTypeSourceInfo(const TypeSourceInfo *TSI) {
  if (!TSI)
    return;
  findInQualType(TSI->getType());
}

static QualType getBaseType(QualType QT) {
  while (true) {
    QT = QualType(QT->getUnqualifiedDesugaredType(), 0);
    QualType Pointee = QT->getPointeeType();
    if (!Pointee.isNull()) {
      QT = Pointee;
      continue;
    }
    if (const ArrayType *Array = QT->getAsArrayTypeUnsafe()) {
      QT = Array->getElementType();
      continue;
    }
    return QT;
  }
}

void DependencyMap::findInQualTypeDefinition(QualType QT) {
  QualType BaseQT = getBaseType(QT);
  if (const TagType *TT = dyn_cast<TagType>(BaseQT.getTypePtr())) {
    findInDecl(TT->getDecl()->getDefinition());
  } else if (const TemplateSpecializationType *TST =
                 dyn_cast<TemplateSpecializationType>(BaseQT.getTypePtr())) {
    // Dig deep to find a definition.
    for (const TemplateArgument &TA : *TST)
      findInTemplateArgument(&TA);
    TemplateName TN = TST->getTemplateName();
    if (TemplateDecl *TD = TN.getAsTemplateDecl()) {
      NamedDecl *ND = TD->getTemplatedDecl();
      if (CXXRecordDecl *CXXRD = dyn_cast_or_null<CXXRecordDecl>(ND))
        findInDecl(CXXRD->getDefinition());
    }
  }
  findInQualType(QT);
}

void DependencyMap::findInTypeSourceInfoDefinition(const TypeSourceInfo *TSI) {
  if (!TSI)
    return;
  findInQualTypeDefinition(TSI->getType());
}

void DependencyMap::findInNestedNameSpecifier(const NestedNameSpecifier *cNNS) {
  NestedNameSpecifier *NNS = const_cast<NestedNameSpecifier *>(cNNS);
  if (!NNS)
    return;
  DependentType LastDependent;
  if (findProlog(NNS, LastDependent))
    return;

  findInNestedNameSpecifier(NNS->getPrefix());
  findInDecl(NNS->getRawDecl());
  findInType(NNS->getAsType());

  CurrentDependent = LastDependent;
}

// Return true if this template arg depends on UD.Types.
void DependencyMap::findInTemplateArgument(const TemplateArgument *cTA) {
  TemplateArgument *TA = const_cast<TemplateArgument *>(cTA);
  if (!TA)
    return;
  switch (TA->getKind()) {
  case TemplateArgument::ArgKind::Null:
  case TemplateArgument::ArgKind::NullPtr:
  case TemplateArgument::ArgKind::Integral:
    break;
  case TemplateArgument::ArgKind::Type:
    findInQualType(TA->getAsType());
    break;
  case TemplateArgument::ArgKind::Declaration:
    findInDecl(TA->getAsDecl());
    findInQualType(TA->getParamTypeForDecl());
    break;
  case TemplateArgument::ArgKind::Template:
  case TemplateArgument::ArgKind::TemplateExpansion:
    findInTemplateName(TA->getAsTemplateOrTemplatePattern());
    break;
  case TemplateArgument::ArgKind::Expression:
    findInStmt(TA->getAsExpr());
    break;
  case TemplateArgument::ArgKind::Pack:
    for (TemplateArgument T : TA->pack_elements())
      findInTemplateArgument(&T);
    break;
  }
}

void DependencyMap::findInTemplateArgumentList(
    const TemplateArgumentList &TAL) {
  for (unsigned i = 0, e = TAL.size(); i < e; ++i)
    findInTemplateArgument(&TAL[i]);
}

void DependencyMap::findInTemplateName(TemplateName TN) {
  switch (TN.getKind()) {
  case TemplateName::NameKind::Template: {
    TemplateDecl *TD = TN.getAsTemplateDecl();
    findInDecl(TD);
    break;
  }
  case TemplateName::NameKind::OverloadedTemplate: {
    OverloadedTemplateStorage *OTS = TN.getAsOverloadedTemplate();
    for (NamedDecl *ND : *OTS)
      findInDecl(ND);
    break;
  }
  case TemplateName::NameKind::QualifiedTemplate: {
    QualifiedTemplateName *QTN = TN.getAsQualifiedTemplateName();
    findInNestedNameSpecifier(QTN->getQualifier());
    findInDecl(QTN->getDecl());
    break;
  }
  case TemplateName::NameKind::DependentTemplate: {
    DependentTemplateName *DTN = TN.getAsDependentTemplateName();
    findInNestedNameSpecifier(DTN->getQualifier());
    break;
  }
  case TemplateName::NameKind::SubstTemplateTemplateParm: {
    SubstTemplateTemplateParmStorage *STTPS =
        TN.getAsSubstTemplateTemplateParm();
    findInDecl(STTPS->getParameter());
    TemplateName TN = STTPS->getReplacement();
    findInTemplateName(TN);
    break;
  }
  case TemplateName::NameKind::SubstTemplateTemplateParmPack: {
    SubstTemplateTemplateParmPackStorage *STTPPS =
        TN.getAsSubstTemplateTemplateParmPack();
    findInDecl(STTPPS->getParameterPack());
    TemplateArgument TA = STTPPS->getArgumentPack();
    findInTemplateArgument(&TA);
    break;
  }
  }
}

void DependencyMap::findInTemplateParameterList(TemplateParameterList *TPL) {
  for (NamedDecl *ND : *TPL)
    findInDecl(ND);
}

void DependencyMap::findInExpr(const Expr *cE) {
  assert(cE);
  // Do not findInStmt or findInExpr here, these are processed in children().
  Expr *E = const_cast<Expr *>(cE);
  QualType QT = E->getType();
  findInQualType(QT);
  // See ScalarExprEmitter::VisitCastExpr.
  if (CastExpr *CE = dyn_cast<CastExpr>(E)) {
    if (ExplicitCastExpr *ECE = dyn_cast<ExplicitCastExpr>(CE))
      findInTypeSourceInfo(ECE->getTypeInfoAsWritten());
    CastKind Kind = CE->getCastKind();
    switch (Kind) {
    case CK_BaseToDerived:
    case CK_BitCast:
    case CK_UncheckedDerivedToBase:
    case CK_DerivedToBase:
    case CK_Dynamic: {
      findInQualTypeDefinition(CE->getSubExpr()->getType());
      for (auto I = CE->path_begin(), E = CE->path_end(); I != E; ++I)
        findInTypeSourceInfoDefinition((*I)->getTypeSourceInfo());
      break;
    }
    default: {
      for (auto I = CE->path_begin(), E = CE->path_end(); I != E; ++I)
        findInTypeSourceInfo((*I)->getTypeSourceInfo());
      break;
    }
    }
  }
  // See lib/AST/StmtProfile.cpp.
  switch (E->getStmtClass()) {
  // Expr.h
  case Stmt::StmtClass::OpaqueValueExprClass:
    break;
  case Stmt::StmtClass::DeclRefExprClass: {
    DeclRefExpr *DRE = cast<DeclRefExpr>(E);
    findInNestedNameSpecifier(DRE->getQualifier());
    findInDecl(DRE->getDecl());
    break;
  }
  case Stmt::StmtClass::PredefinedExprClass:
  case Stmt::StmtClass::IntegerLiteralClass:
  case Stmt::StmtClass::CharacterLiteralClass:
  case Stmt::StmtClass::FloatingLiteralClass:
  case Stmt::StmtClass::ImaginaryLiteralClass:
  case Stmt::StmtClass::StringLiteralClass:
  case Stmt::StmtClass::ParenExprClass:
    break;
  case Stmt::StmtClass::UnaryOperatorClass: {
    UnaryOperator *UE = cast<UnaryOperator>(E);
    if (UE->isIncrementDecrementOp())
      findInQualTypeDefinition(UE->getType());
    break;
  }
  case Stmt::StmtClass::BinaryOperatorClass: {
    BinaryOperator *BE = cast<BinaryOperator>(E);
    if (BE->isAdditiveOp()) {
      findInQualTypeDefinition(BE->getLHS()->getType());
      findInQualTypeDefinition(BE->getRHS()->getType());
    }
    break;
  }
  case Stmt::StmtClass::CompoundAssignOperatorClass: {
    CompoundAssignOperator *CAO = cast<CompoundAssignOperator>(E);
    findInQualTypeDefinition(CAO->getComputationLHSType());
    break;
  }
  case Stmt::StmtClass::OffsetOfExprClass: {
    OffsetOfExpr *OOE = cast<OffsetOfExpr>(E);
    findInTypeSourceInfo(OOE->getTypeSourceInfo());
    unsigned n = OOE->getNumComponents();
    for (unsigned i = 0; i < n; ++i) {
      const OffsetOfNode &ON = OOE->getComponent(i);
      if (ON.getKind() == OffsetOfNode::Field)
        findInDecl(ON.getField());
    }
    break;
  }
  case Stmt::StmtClass::UnaryExprOrTypeTraitExprClass: {
    UnaryExprOrTypeTraitExpr *UEOTTE = cast<UnaryExprOrTypeTraitExpr>(E);
    if (UEOTTE->isArgumentType())
      findInTypeSourceInfo(UEOTTE->getArgumentTypeInfo());
    break;
  }
  case Stmt::StmtClass::ArraySubscriptExprClass: {
    ArraySubscriptExpr *ASE = cast<ArraySubscriptExpr>(E);
    QualType QT = ASE->getLHS()->getType();
    findInQualTypeDefinition(QT);
    break;
  }
  case Stmt::StmtClass::MemberExprClass: {
    MemberExpr *ME = cast<MemberExpr>(E);
    findInDecl(ME->getMemberDecl());
    findInNestedNameSpecifier(ME->getQualifier());
    break;
  }
  case Stmt::StmtClass::CompoundLiteralExprClass: {
    CompoundLiteralExpr *CLE = cast<CompoundLiteralExpr>(E);
    findInTypeSourceInfo(CLE->getTypeSourceInfo());
    break;
  }
#if 0
  case Stmt::StmtClass::CXXReinterpretCastExprClass:
  case Stmt::StmtClass::CXXConstCastExprClass:
  case Stmt::StmtClass::CXXFunctionalCastExprClass: {
    ExplicitCastExpr *ECE = cast<ExplicitCastExpr>(E);
    findInTypeSourceInfo(ECE->getTypeInfoAsWritten());
    break;
  }
  case Stmt::StmtClass::CStyleCastExprClass:
  case Stmt::StmtClass::CXXStaticCastExprClass:
  case Stmt::StmtClass::CXXDynamicCastExprClass: {
    // Dynamic cast class requires the TypeInfo.
    ExplicitCastExpr *ECE = cast<ExplicitCastExpr>(E);
    findInTypeSourceInfoDefinition(ECE->getTypeInfoAsWritten());
    break;
  }
#endif
  case Stmt::StmtClass::AddrLabelExprClass: {
    AddrLabelExpr *ALE = cast<AddrLabelExpr>(E);
    findInDecl(ALE->getLabel());
    break;
  }
  case Stmt::StmtClass::BlockExprClass: {
    BlockExpr *BE = cast<BlockExpr>(E);
    findInDecl(BE->getBlockDecl());
    break;
  }
  // ExprCXX.h
  case Stmt::StmtClass::CXXOperatorCallExprClass:
  case Stmt::StmtClass::CXXMemberCallExprClass:
  case Stmt::StmtClass::CUDAKernelCallExprClass:
  case Stmt::StmtClass::UserDefinedLiteralClass:
  case Stmt::StmtClass::CXXBoolLiteralExprClass:
  case Stmt::StmtClass::CXXNullPtrLiteralExprClass:
  case Stmt::StmtClass::CXXStdInitializerListExprClass:
    break;
  case Stmt::StmtClass::CXXTypeidExprClass: {
    CXXTypeidExpr *CXXTE = cast<CXXTypeidExpr>(E);
    if (CXXTE->isTypeOperand())
      findInTypeSourceInfo(CXXTE->getTypeOperandSourceInfo());
    else
      findInStmt(CXXTE->getExprOperand());
    break;
  }
  case Stmt::StmtClass::MSPropertyRefExprClass: {
    MSPropertyRefExpr *MSPRE = cast<MSPropertyRefExpr>(E);
    findInDecl(MSPRE->getPropertyDecl());
    break;
  }
  case Stmt::StmtClass::MSPropertySubscriptExprClass:
    break;
  case Stmt::StmtClass::CXXUuidofExprClass: {
    CXXUuidofExpr *CXXUE = cast<CXXUuidofExpr>(E);
    if (CXXUE->isTypeOperand())
      findInTypeSourceInfo(CXXUE->getTypeOperandSourceInfo());
    break;
  }
  case Stmt::StmtClass::CXXThisExprClass:
  case Stmt::StmtClass::CXXThrowExprClass:
    break;
  case Stmt::StmtClass::CXXDefaultArgExprClass: {
    CXXDefaultArgExpr *CXXDAE = cast<CXXDefaultArgExpr>(E);
    findInDecl(CXXDAE->getParam());
    break;
  }
  case Stmt::StmtClass::CXXDefaultInitExprClass: {
    CXXDefaultInitExpr *CXXDIE = cast<CXXDefaultInitExpr>(E);
    FieldDecl *FD = CXXDIE->getField();
    findInDecl(FD);
    break;
  }
  case Stmt::StmtClass::CXXBindTemporaryExprClass: {
    CXXBindTemporaryExpr *CXXBTE = cast<CXXBindTemporaryExpr>(E);
    CXXTemporary *CXXT = CXXBTE->getTemporary();
    CXXDestructorDecl *CXXDD =
        const_cast<CXXDestructorDecl *>(CXXT->getDestructor());
    findInDecl(CXXDD);
    break;
  }
  case Stmt::StmtClass::CXXConstructExprClass:
  case Stmt::StmtClass::CXXTemporaryObjectExprClass: {
    CXXConstructExpr *CXXCE = cast<CXXConstructExpr>(E);
    findInDecl(CXXCE->getConstructor());
    if (CXXTemporaryObjectExpr *CXXTOE = dyn_cast<CXXTemporaryObjectExpr>(E))
      findInTypeSourceInfo(CXXTOE->getTypeSourceInfo());
    break;
  }
  case Stmt::StmtClass::LambdaExprClass: {
    LambdaExpr *LE = cast<LambdaExpr>(E);
    findInDecl(LE->getLambdaClass());
    for (auto &C : LE->explicit_captures()) {
      auto Kind = C.getCaptureKind();
      if (Kind == LCK_ByRef || Kind == LCK_ByCopy)
        findInDecl(C.getCapturedVar());
    }
    break;
  }
  case Stmt::StmtClass::CXXScalarValueInitExprClass: {
    CXXScalarValueInitExpr *CXXSVIE = cast<CXXScalarValueInitExpr>(E);
    findInTypeSourceInfo(CXXSVIE->getTypeSourceInfo());
    break;
  }
  case Stmt::StmtClass::CXXNewExprClass: {
    CXXNewExpr *CXXNE = cast<CXXNewExpr>(E);
    findInTypeSourceInfo(CXXNE->getAllocatedTypeSourceInfo());
    findInDecl(CXXNE->getOperatorNew());
    findInDecl(CXXNE->getOperatorDelete());
    break;
  }
  case Stmt::StmtClass::CXXDeleteExprClass: {
    CXXDeleteExpr *CXXDE = cast<CXXDeleteExpr>(E);
    findInDecl(CXXDE->getOperatorDelete());
    break;
  }
  case Stmt::StmtClass::CXXPseudoDestructorExprClass: {
    CXXPseudoDestructorExpr *CXXPDE = cast<CXXPseudoDestructorExpr>(E);
    findInNestedNameSpecifier(CXXPDE->getQualifier());
    findInTypeSourceInfo(CXXPDE->getScopeTypeInfo());
    findInTypeSourceInfo(CXXPDE->getDestroyedTypeInfo());
    break;
  }
  case Stmt::StmtClass::TypeTraitExprClass: {
    break;
  }
  case Stmt::StmtClass::ArrayTypeTraitExprClass: {
    ArrayTypeTraitExpr *ATTE = cast<ArrayTypeTraitExpr>(E);
    findInTypeSourceInfo(ATTE->getQueriedTypeSourceInfo());
    break;
  }
  case Stmt::StmtClass::ExpressionTraitExprClass: {
    break;
  }
  case Stmt::StmtClass::UnresolvedLookupExprClass:
  case Stmt::StmtClass::UnresolvedMemberExprClass: {
    OverloadExpr *OE = cast<OverloadExpr>(E);
// eraseFromOverloadExprs will fix these later.
#if 0
    for (auto I : OE->decls()) {
      NamedDecl *ND = &*I;
      findInDecl(ND);
    }
#endif
    if (const TemplateArgumentLoc *TAL = OE->getTemplateArgs())
      for (unsigned i = 0, N = OE->getNumTemplateArgs(); i < N; ++i, ++TAL)
        findInTemplateArgument(&TAL->getArgument());
    findInNestedNameSpecifier(OE->getQualifier());
    if (UnresolvedMemberExpr *UME = dyn_cast<UnresolvedMemberExpr>(OE))
      findInQualType(UME->getBaseType());
    else if (UnresolvedLookupExpr *ULE = dyn_cast<UnresolvedLookupExpr>(OE))
      findInDecl(ULE->getNamingClass());
    break;
  }
  case Stmt::StmtClass::DependentScopeDeclRefExprClass: {
    DependentScopeDeclRefExpr *DSDRE = cast<DependentScopeDeclRefExpr>(E);
    findInNestedNameSpecifier(DSDRE->getQualifier());
    for (const TemplateArgumentLoc &TAL : DSDRE->template_arguments()) {
      const TemplateArgument &TA = TAL.getArgument();
      findInTemplateArgument(&TA);
    }
    break;
  }
  case Stmt::StmtClass::ExprWithCleanupsClass:
    break;
  case Stmt::StmtClass::CXXUnresolvedConstructExprClass: {
    CXXUnresolvedConstructExpr *CXXUCE = cast<CXXUnresolvedConstructExpr>(E);
    findInTypeSourceInfo(CXXUCE->getTypeSourceInfo());
    break;
  }
  case Stmt::StmtClass::CXXDependentScopeMemberExprClass: {
    CXXDependentScopeMemberExpr *CXXDSME = cast<CXXDependentScopeMemberExpr>(E);
    if (!CXXDSME->isImplicitAccess())
      findInQualType(CXXDSME->getBaseType());
    findInNestedNameSpecifier(CXXDSME->getQualifier());
    findInDecl(CXXDSME->getFirstQualifierFoundInScope());
    break;
  }
  case Stmt::StmtClass::CXXNoexceptExprClass:
  case Stmt::StmtClass::PackExpansionExprClass:
    break;
  case Stmt::StmtClass::SizeOfPackExprClass: {
    SizeOfPackExpr *SOPE = cast<SizeOfPackExpr>(E);
    findInDecl(SOPE->getPack());
    break;
  }
  case Stmt::StmtClass::SubstNonTypeTemplateParmPackExprClass: {
    SubstNonTypeTemplateParmPackExpr *SNTTPPE =
        cast<SubstNonTypeTemplateParmPackExpr>(E);
    findInDecl(SNTTPPE->getParameterPack());
    break;
  }
  case Stmt::StmtClass::SubstNonTypeTemplateParmExprClass: {
    SubstNonTypeTemplateParmExpr *SNTTPE =
        cast<SubstNonTypeTemplateParmExpr>(E);
    findInDecl(SNTTPE->getParameter());
    break;
  }
  case Stmt::StmtClass::FunctionParmPackExprClass: {
    FunctionParmPackExpr *FPPE = cast<FunctionParmPackExpr>(E);
    findInDecl(FPPE->getParameterPack());
    for (ParmVarDecl *PVD : *FPPE)
      findInDecl(PVD);
    break;
  }
  case Stmt::StmtClass::MaterializeTemporaryExprClass: {
    MaterializeTemporaryExpr *MTE = cast<MaterializeTemporaryExpr>(E);
    findInDecl(MTE->getExtendingDecl());
    break;
  }
  case Stmt::StmtClass::CXXFoldExprClass:
  case Stmt::StmtClass::CoawaitExprClass:
  case Stmt::StmtClass::CoyieldExprClass:
    break;
  // ExprObjc.h
  case Stmt::StmtClass::ObjCProtocolExprClass: {
    ObjCProtocolExpr *OCPE = cast<ObjCProtocolExpr>(E);
    findInDecl(OCPE->getProtocol());
    break;
  }
  case Stmt::StmtClass::ObjCIvarRefExprClass: {
    ObjCIvarRefExpr *OCRE = cast<ObjCIvarRefExpr>(E);
    findInDecl(OCRE->getDecl());
    break;
  }
  case Stmt::StmtClass::ObjCPropertyRefExprClass: {
    ObjCPropertyRefExpr *OCPRE = cast<ObjCPropertyRefExpr>(E);
    if (OCPRE->isImplicitProperty()) {
      findInDecl(OCPRE->getImplicitPropertyGetter());
      findInDecl(OCPRE->getImplicitPropertySetter());
    } else {
      findInDecl(OCPRE->getExplicitProperty());
    }
    break;
  }
  case Stmt::StmtClass::ObjCSubscriptRefExprClass: {
    ObjCSubscriptRefExpr *OCSRE = cast<ObjCSubscriptRefExpr>(E);
    findInDecl(OCSRE->getAtIndexMethodDecl());
    findInDecl(OCSRE->setAtIndexMethodDecl());
    break;
  }
  case Stmt::StmtClass::ObjCMessageExprClass: {
    ObjCMessageExpr *OCME = cast<ObjCMessageExpr>(E);
    findInDecl(OCME->getMethodDecl());
    break;
  }
  // ExprOpenMP.h
  case Stmt::StmtClass::OMPArraySectionExprClass:
    break;
  default:
    break;
  }
}

void DependencyMap::findInStmt(const Stmt *S) {
  if (!S)
    return;
  for (const Stmt *Child : S->children())
    findInStmt(Child);
  if (const Expr *E = dyn_cast<Expr>(S)) {
    findInExpr(E);
    return;
  }
  switch (S->getStmtClass()) {
  case Stmt::StmtClass::DeclStmtClass:
    for (const Decl *D : cast<DeclStmt>(S)->decls())
      findInDecl(D);
    break;
  case Stmt::StmtClass::LabelStmtClass:
    findInDecl(cast<LabelStmt>(S)->getDecl());
    break;
  case Stmt::StmtClass::IfStmtClass:
    findInDecl(cast<IfStmt>(S)->getConditionVariable());
    break;
  case Stmt::StmtClass::SwitchStmtClass:
    findInDecl(cast<SwitchStmt>(S)->getConditionVariable());
    break;
  case Stmt::StmtClass::WhileStmtClass:
    findInDecl(cast<WhileStmt>(S)->getConditionVariable());
    break;
  case Stmt::StmtClass::ForStmtClass:
    findInDecl(cast<ForStmt>(S)->getConditionVariable());
    break;
  case Stmt::StmtClass::GotoStmtClass:
    findInDecl(cast<GotoStmt>(S)->getLabel());
    break;
  case Stmt::StmtClass::IndirectGotoStmtClass:
    findInDecl(cast<IndirectGotoStmt>(S)->getConstantTarget());
    break;
  case Stmt::StmtClass::ReturnStmtClass:
    findInDecl(cast<ReturnStmt>(S)->getNRVOCandidate());
    break;
  case Stmt::StmtClass::CapturedStmtClass:
    findInDecl(cast<CapturedStmt>(S)->getCapturedDecl());
    findInDecl(cast<CapturedStmt>(S)->getCapturedRecordDecl());
    break;
  case Stmt::StmtClass::CXXCatchStmtClass:
    findInDecl(cast<CXXCatchStmt>(S)->getExceptionDecl());
    break;
  case Stmt::StmtClass::CXXForRangeStmtClass:
    findInDecl(cast<CXXForRangeStmt>(S)->getLoopVariable());
    break;
  case Stmt::StmtClass::CoroutineBodyStmtClass:
    findInDecl(cast<CoroutineBodyStmt>(S)->getPromiseDecl());
    break;
  default:
    break;
  }
}

void DependencyMap::findInDecl(const Decl *cD) {
  Decl *D = const_cast<Decl *>(cD);
  if (!D)
    return;
  if (isa<TranslationUnitDecl>(D))
    return;
  DependentType LastDependent;
  if (findProlog(D, LastDependent))
    return;
  Visit(D);
  const DeclContext *SemanticDC = D->getDeclContext();
  const DeclContext *LexicalDC = D->getLexicalDeclContext();
  if (SemanticDC)
    findInDeclContext(SemanticDC);
  // See class-friend-itself.cpp test.
  if (LexicalDC && LexicalDC != SemanticDC)
    findInDeclContext(LexicalDC);
  CurrentDependent = LastDependent;
}

void DependencyMap::findInFileEntry(const FileEntry *FE) {
  findInFileEntry(const_cast<FileEntry *>(FE));
}

static bool dependsOnContext(const DeclContext *DC) {
  if (DC->isFileContext())
    return true;
  if (DC->getDeclKind() == Decl::Kind::CXXRecord)
    if (DC->getParent()->isFileContext())
      return true;
  return false;
}

static bool dependsOn(const Decl *D) {
  if (!dependsOnContext(D->getLexicalDeclContext()->getRedeclContext()))
    return false;
  if (auto *CTSD = dyn_cast<ClassTemplateSpecializationDecl>(D))
    return CTSD->getTemplateSpecializationKind() == TSK_ExplicitSpecialization;
  if (auto *VTSD = dyn_cast<VarTemplateSpecializationDecl>(D))
    return VTSD->getTemplateSpecializationKind() == TSK_ExplicitSpecialization;
  if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
    return FD->getTemplateSpecializationKind() == TSK_ExplicitSpecialization ||
           FD->getTemplatedKind() ==
               FunctionDecl::TemplatedKind::TK_NonTemplate;
  }
  if (const VarDecl *VD = dyn_cast<VarDecl>(D))
    return !VD->isOutOfLine() || !VD->isStaticDataMember() ||
           !VD->getInstantiatedFromStaticDataMember();
  return isa<RedeclarableTemplateDecl>(D) || isa<TagDecl>(D) ||
         isa<TypedefNameDecl>(D) || isa<UsingDecl>(D) ||
         isa<UsingDirectiveDecl>(D);
}

void DependencyMap::findInFileEntry(FileEntry *FE) {
  DependentType LastDependent;
  if (findProlog(FE, LastDependent))
    return;
  auto I = FileEntryDependees.find(FE);
  if (I != FileEntryDependees.end())
    for (const FileEntry *Dependee : I->second)
      findInFileEntry(Dependee);
  auto J = FDMapping.find(FE);
  if (J != FDMapping.file_end())
    for (const Decl *D : J->second)
      if (dependsOn(D))
        findInDecl(D);
  CurrentDependent = LastDependent;
}

bool DependencyMap::VisitFriendDecl(const FriendDecl *FD) {
  if (TypeSourceInfo *TSI = FD->getFriendType())
    findInTypeSourceInfo(TSI);
  else
    findInDecl(FD->getFriendDecl());
  unsigned n = FD->getFriendTypeNumTemplateParameterLists();
  for (unsigned i = 0; i < n; ++i)
    findInTemplateParameterList(FD->getFriendTypeTemplateParameterList(i));
  return true;
}

bool DependencyMap::VisitStaticAssertDecl(const StaticAssertDecl *SAD) {
  findInStmt(SAD->getAssertExpr());
  return true;
}

bool DependencyMap::VisitLabelDecl(const LabelDecl *LD) {
  findInStmt(LD->getStmt());
  return true;
}

bool DependencyMap::VisitNamespaceAliasDecl(const NamespaceAliasDecl *NAD) {
  findInNestedNameSpecifier(NAD->getQualifier());
  findInDecl(NAD->getAliasedNamespace());
  return true;
}

bool DependencyMap::VisitNamespaceDecl(const NamespaceDecl *ND) {
#if 0
  // Original namespace will be fixed by unloader.
  findInDecl(NSD->getRawNamespace());
#endif
  return true;
}

bool DependencyMap::VisitTemplateDecl(const TemplateDecl *TD) {
  findInTemplateParameterList(TD->getTemplateParameters());  
  findInDecl(TD->getTemplatedDecl());
  findInStmt(TD->getAssociatedConstraints());
  return true;
}

bool DependencyMap::VisitRedeclarableTemplateDecl(
    const RedeclarableTemplateDecl *RTD) {
  VisitTemplateDecl(RTD);
  // User specializations will be removed from the template.
  return true;
}

bool DependencyMap::VisitFunctionTemplateDecl(const FunctionTemplateDecl *FTD) {
  VisitRedeclarableTemplateDecl(FTD);
  return true;
}

bool DependencyMap::VisitTemplateTemplateParmDecl(
    const TemplateTemplateParmDecl *TTPD) {
  VisitTemplateDecl(TTPD);
  if (TTPD->isExpandedParameterPack()) {
    unsigned n = TTPD->getNumExpansionTemplateParameters();
    for (unsigned i = 0; i < n; ++i) {
      TemplateParameterList *TPL = TTPD->getExpansionTemplateParameters(i);
      findInTemplateParameterList(TPL);
    }
  }
  return true;
}

bool DependencyMap::VisitTypeDecl(const TypeDecl *TD) {
  assert(TD && "TypeDecl can not be nullptr.");
  findInType(TD->getTypeForDecl());
  return true;
}

bool DependencyMap::VisitTagDecl(const TagDecl *TD) {
  VisitTypeDecl(TD);
  const DeclContext *DC = cast<DeclContext>(TD);
  for (Decl *NLD : DC->noload_decls()) {
    if (CXXRecordDecl *CXXRD = dyn_cast<CXXRecordDecl>(NLD))
      if (CXXRD->isLambda())
        continue;
#if 0
    // Needed?
    if (FieldDecl *FD = dyn_cast<FieldDecl>(NLD))
      if (const TagType *TT = FD->getType()->getAs<TagType>())
        findInDecl(TT->getDecl());
#endif
    findInDecl(NLD);
  }
  return true;
}

bool DependencyMap::VisitEnumDecl(const EnumDecl *ED) {
  VisitTagDecl(ED);
  findInDecl(ED->getInstantiatedFromMemberEnum());
  return true;
}

bool DependencyMap::VisitRecordDecl(const RecordDecl *RD) {
  VisitTagDecl(RD);
  return true;
}

bool DependencyMap::VisitCXXRecordDecl(const CXXRecordDecl *CXXRD) {
  VisitRecordDecl(CXXRD);
  // Required for test-ns-template-lambda.cpp. hasDefinition() will be true
  // for all redecls, even declarations.
  if (CXXRD->isCompleteDefinition())
    for (const CXXBaseSpecifier &CXXBS : CXXRD->bases())
      findInTypeSourceInfoDefinition(CXXBS.getTypeSourceInfo());
  return true;
}

bool DependencyMap::VisitClassTemplateSpecializationDecl(
    const ClassTemplateSpecializationDecl *CTSD) {
  VisitCXXRecordDecl(CTSD);
  findInTemplateArgumentList(CTSD->getTemplateArgs());
  if (TypeSourceInfo *TSI = CTSD->getTypeAsWritten())
    findInTypeSourceInfo(TSI);
  llvm::PointerUnion<ClassTemplateDecl *,
                     ClassTemplatePartialSpecializationDecl *>
      Template = CTSD->getSpecializedTemplateOrPartial();
  if (Template.is<ClassTemplateDecl *>()) {
    const ClassTemplateDecl *CTD = Template.get<ClassTemplateDecl *>();
    findInDecl(CTD);
  } else {
    const ClassTemplatePartialSpecializationDecl *CTPSD =
        Template.get<ClassTemplatePartialSpecializationDecl *>();
    findInDecl(CTPSD);
    findInTemplateArgumentList(CTSD->getTemplateInstantiationArgs());
  }
  return true;
}

bool DependencyMap::VisitClassTemplatePartialSpecializationDecl(
    const ClassTemplatePartialSpecializationDecl *CTPSD) {
  VisitClassTemplateSpecializationDecl(CTPSD);
  findInTemplateParameterList(CTPSD->getTemplateParameters());
  return true;
}

bool DependencyMap::VisitTemplateTypeParmDecl(
    const TemplateTypeParmDecl *TTPD) {
  VisitTypeDecl(TTPD);
  if (TTPD->hasDefaultArgument())
    findInQualType(TTPD->getDefaultArgument());
  return true;
}

bool DependencyMap::VisitTypedefNameDecl(const TypedefNameDecl *TND) {
  VisitTypeDecl(TND);
  findInQualType(TND->getUnderlyingType());
  if (TND->isModed())
    findInTypeSourceInfo(TND->getTypeSourceInfo());
  return true;
}

bool DependencyMap::VisitTypedefDecl(const TypedefDecl *TD) {
  VisitTypedefNameDecl(TD);
  return true;
}

bool DependencyMap::VisitTypeAliasDecl(const TypeAliasDecl *TAD) {
  VisitTypedefNameDecl(TAD);
  findInDecl(TAD->getDescribedAliasTemplate());
  return true;
}

bool DependencyMap::VisitUnresolvedUsingTypenameDecl(
    const UnresolvedUsingTypenameDecl *UUTD) {
  VisitTypeDecl(UUTD);
  findInNestedNameSpecifier(UUTD->getQualifier());
  return true;
}

bool DependencyMap::VisitUsingDecl(const UsingDecl *UD) {
  UsingShadowDecl *USD = *UD->shadow_begin();
  findInDecl(USD);
  findInNestedNameSpecifier(UD->getQualifier());
  return true;
}

bool DependencyMap::VisitUsingPackDecl(const UsingPackDecl *UPD) {
  findInDecl(UPD->getInstantiatedFromUsingDecl());
  for (const NamedDecl *ND : UPD->expansions())
    findInDecl(ND);
  return true;
}

bool DependencyMap::VisitUsingShadowDecl(const UsingShadowDecl *USD) {
  findInDecl(USD->getUsingDecl());
  findInDecl(USD->getTargetDecl());
  return true;
}

bool DependencyMap::VisitUsingConstructorUsingShadowDecl(
    ConstructorUsingShadowDecl *CUSD) {
  VisitUsingShadowDecl(CUSD);
  findInDecl(CUSD->getNominatedBaseClassShadowDecl());
  findInDecl(CUSD->getConstructedBaseClassShadowDecl());
  return true;
}

bool DependencyMap::VisitUsingDirectiveDecl(const UsingDirectiveDecl *UDD) {
  findInNestedNameSpecifier(UDD->getQualifier());
  findInDecl(UDD->getNominatedNamespaceAsWritten());
  findInDeclContext(UDD->getCommonAncestor());
  return true;
}

bool DependencyMap::VisitBindingDecl(const BindingDecl *BD) {
  findInStmt(BD->getBinding());
  return true;
}

bool DependencyMap::VisitEnumConstantDecl(const EnumConstantDecl *ECD) {
  findInStmt(ECD->getInitExpr());
  return true;
}

bool DependencyMap::VisitIndirectFieldDecl(const IndirectFieldDecl *IFD) {
  for (NamedDecl *ND : IFD->chain())
    findInDecl(ND);
  return true;
}
bool DependencyMap::VisitUnresolvedUsingValueDecl(
    const UnresolvedUsingValueDecl *UUVD) {
  return true;
}

bool DependencyMap::VisitBlockDecl(const BlockDecl *BD) {
  if (BD->hasBody())
    findInStmt(BD->getBody());
  for (ParmVarDecl *PVD : BD->parameters())
    findInDecl(PVD);
  findInTypeSourceInfo(BD->getSignatureAsWritten());
  return true;
}

bool DependencyMap::VisitCapturedDecl(const CapturedDecl *CD) {
  for (const ImplicitParamDecl *IPD : CD->parameters())
    findInDecl(IPD);
  findInDecl(CD->getContextParam());
  return true;
}

bool DependencyMap::VisitClassScopeFunctionSpecializationDecl(
    const ClassScopeFunctionSpecializationDecl *CSFSD) {
  findInDecl(CSFSD->getSpecialization());
  return true;
}

bool DependencyMap::VisitDeclaratorDecl(const DeclaratorDecl *DD) {
  findInTypeSourceInfo(DD->getTypeSourceInfo());
  findInNestedNameSpecifier(DD->getQualifier());
  for (unsigned i = 0, e = DD->getNumTemplateParameterLists(); i != e; ++i)
    findInTemplateParameterList(DD->getTemplateParameterList(i));
  return true;
}

bool DependencyMap::VisitFieldDecl(const FieldDecl *FD) {
  VisitDeclaratorDecl(FD);
  findInQualType(FD->getType());
  findInStmt(FD->getInClassInitializer());
  return true;
}

bool DependencyMap::VisitFunctionDecl(const FunctionDecl *FD) {
  VisitDeclaratorDecl(FD);
  QualType QT = FD->getType();
  findInQualType(QT);
  // __copy_helper_block_ has no function type.
  if (QT->isFunctionType())
    findInQualType(QT->getAs<FunctionType>()->getReturnType());
  for (ParmVarDecl *PVD : FD->parameters())
    findInDecl(PVD);
  if (const TemplateArgumentList *TAL = FD->getTemplateSpecializationArgs())
    findInTemplateArgumentList(*TAL);
  findInDecl(FD->getDescribedFunctionTemplate());
  findInDecl(FD->getPrimaryTemplate());
  findInDecl(FD->getInstantiatedFromMemberFunction());
  // Body
  if (!FD->doesThisDeclarationHaveABody())
    return true;
  if (const CXXConstructorDecl *CXXCD = dyn_cast<CXXConstructorDecl>(FD)) {
    for (CXXCtorInitializer *CXXCI : CXXCD->inits()) {
      findInTypeSourceInfo(CXXCI->getTypeSourceInfo());
      findInDecl(CXXCI->getAnyMember());
      findInStmt(CXXCI->getInit());
    }
  } else if (const CXXDestructorDecl *CXXDD = dyn_cast<CXXDestructorDecl>(FD)) {
    findInDecl(CXXDD->getRawOperatorDelete());
  }
  findInStmt(FD->getBody());
  return true;
}

bool DependencyMap::VisitMSPropertyDecl(const MSPropertyDecl *MSPD) {
  VisitDeclaratorDecl(MSPD);
  return true;
}

bool DependencyMap::VisitNonTypeTemplateParmDecl(
    const NonTypeTemplateParmDecl *NTTPD) {
  VisitDeclaratorDecl(NTTPD);
  if (NTTPD->isExpandedParameterPack())
    for (unsigned i = 0, n = NTTPD->getNumExpansionTypes(); i < n; ++i)
      findInQualType(NTTPD->getExpansionType(i));
  if (NTTPD->hasDefaultArgument())
    findInStmt(NTTPD->getDefaultArgument());
  return true;
}

bool DependencyMap::VisitVarDecl(const VarDecl *VRD) {
  VisitDeclaratorDecl(VRD);
  findInStmt(VRD->getInit());
  findInDecl(VRD->getInstantiatedFromStaticDataMember());
  return true;
}

bool DependencyMap::VisitParmVarDecl(const ParmVarDecl *PVD) {
  VisitVarDecl(PVD);
  findInQualType(PVD->getOriginalType());
  return true;
}

bool DependencyMap::VisitVarTemplateSpecializationDecl(
    const VarTemplateSpecializationDecl *VTSD) {
  VisitVarDecl(VTSD);
  findInTemplateArgumentList(VTSD->getTemplateArgs());
  return true;
}

bool DependencyMap::VisitDecompositionDecl(const DecompositionDecl *DD) {
  VisitVarDecl(DD);
  for (BindingDecl *BD : DD->bindings())
    findInDecl(BD);
  return true;
}

} // namespace clang
