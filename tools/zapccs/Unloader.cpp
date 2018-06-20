// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "Unloader.h"
#include "ReplaceDeclInMap.h"
#include "StringConst.h"

#include "../lib/CodeGen/CodeGenModule.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Sema.h"

using namespace llvm;
namespace clang {

void Unloader::ReplaceDeclInMap(NamedDecl *OriginalND, NamedDecl *ReplacementND,
                                DeclContext *DC) {
  if (OriginalND == ReplacementND)
    return;
  if (!DC)
    DC = OriginalND->getDeclContext();
  do {
    DC = DC->getPrimaryContext();
#define REPLACE_DEBUG 0
#if REPLACE_DEBUG && !defined(NDEBUG)
    DeclarationName Name = OriginalND->getDeclName();
    bool Print = OriginalND->getNameAsString() == "Bug792end";
    if (Print) {
      llvm::errs() << "\n==== ReplaceDeclInMap ====\nReplacing:\n";
      OriginalND->dumpColor();
      llvm::errs() << "With:\n";
      if (ReplacementND) {
        ReplacementND->dumpColor();
        llvm::errs() << "Whose DeclContext is:\n";
        DeclContext *ReplacementDC = ReplacementND->getDeclContext();
        if (const Decl *D = dyn_cast<Decl>(ReplacementDC))
          D->dumpColor();
        else
          llvm::errs() << ReplacementDC;
        DeclContext *LexicalDC = ReplacementND->getLexicalDeclContext();
        if (LexicalDC != ReplacementDC) {
          llvm::errs() << "Whose LexicalDeclContext is:\n";
          if (const Decl *D = dyn_cast<Decl>(LexicalDC))
            D->dumpColor();
          else
            llvm::errs() << LexicalDC;
        }

      } else
        llvm::errs() << "(nullptr)\n";
      llvm::errs() << "In:\n";
      if (const Decl *D = dyn_cast<Decl>(DC))
        D->dumpColor();
      else
        llvm::errs() << DC;
      llvm::errs() << "\n";
    }
    bool Removed =
#endif
        ReplaceDeclInMapImpl(OriginalND, ReplacementND, DC);
#if REPLACE_DEBUG && !defined(NDEBUG)
    if (Print) {
      if (!Removed)
        llvm::errs() << "NOT ";
      llvm::errs() << "FOUND\n";
      llvm::errs() << "\n";
    }
#endif
    // DeclContext::makeDeclVisibleInContextWithFlags
  } while ((DC->isTransparentContext() || DC->isNamespace()) &&
           (DC = DC->getParent()));
} // ReplaceDeclInMap

template <typename T>
void eraseFromLazyVector(T &V, SmallPtrSetImpl<Decl *> &DeclSet) {
  auto I = std::remove_if(
      V.begin(nullptr, true), V.end(), [&](typename T::value_type D) {
        return DeclSet.count(const_cast<Decl *>(cast<Decl>(D)));
      });
  V.erase(I, V.end());
}

void Unloader::unloadVectors() {
  S.SpecialMemberCache.clear();
  eraseFromLazyVector(S.UnusedFileScopedDecls, Visited);
  eraseFromLazyVector(S.ExtVectorDecls, Visited);
  eraseFromLazyVector(S.TentativeDefinitions, Visited);
  eraseFromLazyVector(S.DelegatingCtorDecls, Visited);
  S.LateParsedTemplateMap.remove_if(
      [&](std::pair<const FunctionDecl *, std::unique_ptr<LateParsedTemplate>>
              &Pair) {
        return Visited.count(const_cast<FunctionDecl *>(Pair.first));
      });
  S.KnownNamespaces.remove_if([&](std::pair<NamespaceDecl *, bool> Pair) {
    return Visited.count(Pair.first);
  });
  for (auto &Pair : S.CachedPendingInstantiations)
    eraseDenseMap(Pair.second, [&](std::pair<ValueDecl *, SourceLocation> &I) {
      return Visited.count(I.first);
    });
  S.UndefinedButUsed.remove_if([&](std::pair<NamedDecl *, SourceLocation> &I) {
    return Visited.count(I.first);
  });
  eraseDenseMap(
      C.InstantiationChanges,
      [&](std::pair<const NamedDecl *, ASTContext::InstantiationChange> &I) {
        return Visited.count(const_cast<NamedDecl *>(I.first));
      });
}

void Unloader::unloadDeclContexts() {
  // DeclContext *TUDC = S.CachedTUScope->getEntity();
  // First pass: collect all DeclContexts.
  SmallPtrSet<DeclContext *, 1> DCs;
  DeclContext *ExternCContextDecl = S.getASTContext().getExternCContextDecl();
  DCs.insert(ExternCContextDecl);
  // Sema::DeduceAutoType creates TemplateTypeParm without DeclContext.
  for (Decl *D : Visited) {
    if (DeclContext *DC = D->getLexicalDeclContext())
      DCs.insert(DC);
    // We use this bit (unused in C++) to identify
    // dead decls while scanning the declcontext.
    D->setTopLevelDeclInObjCContainer();
  }
  // Second pass: loop over DeclContext and clean the linked lists.
  for (DeclContext *DC : DCs) {
    Decl *First = DC->getFirstDeclDC();
    Decl *Node = First;
    Decl *Prev = nullptr;
    while (Node) {
      Decl *Next = Node->getNextDeclInContext();
      if (Node->isTopLevelDeclInObjCContainer()) {
        if (Node == First) {
          First = Next;
        } else {
          Prev->setNextDeclInContext(Next);
        }
        Node->setNextDeclInContext(nullptr);
      } else {
        Prev = Node;
      }
      Node = Next;
    }
    DC->getFirstDeclDC() = First;
    DC->getLastDeclDC() = Prev;
  }
}

void Unloader::eraseDeclsASTContext() {
  for (Decl *D : Visited) {
    C.DeclAttrs.erase(D);
    C.MergedDecls.erase(D);
#if 0
      // cleared in ASTContext::clearComments().
      C.RedeclComments.erase(D);
      C.ParsedComments.erase(D);
#endif
    if (NamedDecl *ND = dyn_cast<NamedDecl>(D)) {
      C.MangleNumbers.erase(ND);
      C.MergedDefModules.erase(ND);
    }
    if (RecordDecl *RD = dyn_cast<RecordDecl>(D)) {
      C.ASTRecordLayouts.erase(RD);
      if (CXXRecordDecl *CXXRD = dyn_cast<CXXRecordDecl>(D))
        C.KeyFunctions.erase(CXXRD);
    } else if (VarDecl *VD = dyn_cast<VarDecl>(D)) {
      C.BlockVarCopyInits.erase(VD);
      C.TemplateOrInstantiation.erase(VD);
      C.StaticLocalNumbers.erase(VD);
      C.ParamIndices.erase(VD);
    } else if (FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
      C.ClassScopeSpecializationPattern.erase(FD);
      if (CXXMethodDecl *CXXMD = dyn_cast<CXXMethodDecl>(D))
        C.OverriddenMethods.erase(CXXMD);
    } else if (UsingDecl *UD = dyn_cast<UsingDecl>(D)) {
      C.InstantiatedFromUsingDecl.erase(UD);
    } else if (UsingShadowDecl *USD = dyn_cast<UsingShadowDecl>(D)) {
      C.InstantiatedFromUsingShadowDecl.erase(USD);
    } else if (FieldDecl *FD = dyn_cast<FieldDecl>(D)) {
      C.InstantiatedFromUnnamedFieldDecl.erase(FD);
    } else if (ObjCContainerDecl *OCCD = dyn_cast<ObjCContainerDecl>(D)) {
      C.ObjCLayouts.erase(OCCD);
      C.ObjCImpls.erase(OCCD);
    } else if (ObjCMethodDecl *OCMD = dyn_cast<ObjCMethodDecl>(D)) {
      C.ObjCMethodRedecls.erase(OCMD);
    }
  }
}
static void RemoveNode(ASTContext &C, Type *T) {
  switch (T->getTypeClass()) {
  case Type::TypeClass::Builtin:
  case Type::TypeClass::Record:
  case Type::TypeClass::Enum:
  case Type::TypeClass::InjectedClassName:
  case Type::TypeClass::UnaryTransform:
  case Type::TypeClass::TypeOf:
  case Type::TypeClass::UnresolvedUsing:
  case Type::TypeClass::Typedef:
  case Type::TypeClass::VariableArray:
    break;
  case Type::TypeClass::TypeOfExpr:
    if (T->isDependentType() && T->isCanonicalUnqualified()) {
      DependentTypeOfExprType *DTOET =
          const_cast<DependentTypeOfExprType *>(cast<DependentTypeOfExprType>(
              T->getCanonicalTypeInternal().getTypePtr()));
      C.DependentTypeOfExprTypes.RemoveNode(DTOET);
    }
    break;
  case Type::TypeClass::Decltype:
    if (T->isDependentType() && T->isCanonicalUnqualified()) {
      DependentDecltypeType *DDT =
          const_cast<DependentDecltypeType *>(cast<DependentDecltypeType>(
              T->getCanonicalTypeInternal().getTypePtr()));
      C.DependentDecltypeTypes.RemoveNode(DDT);
    }
    break;
  case Type::TypeClass::Complex:
    C.ComplexTypes.RemoveNode(cast<ComplexType>(T));
    break;
  case Type::TypeClass::Pointer:
    C.PointerTypes.RemoveNode(cast<PointerType>(T));
    break;
  case Type::TypeClass::Decayed:
  case Type::TypeClass::Adjusted:
    C.AdjustedTypes.RemoveNode(cast<AdjustedType>(T));
    break;
  case Type::TypeClass::BlockPointer:
    C.BlockPointerTypes.RemoveNode(cast<BlockPointerType>(T));
    break;
  case Type::TypeClass::LValueReference:
    C.LValueReferenceTypes.RemoveNode(cast<LValueReferenceType>(T));
    break;
  case Type::TypeClass::RValueReference:
    C.RValueReferenceTypes.RemoveNode(cast<RValueReferenceType>(T));
    break;
  case Type::TypeClass::MemberPointer:
    C.MemberPointerTypes.RemoveNode(cast<MemberPointerType>(T));
    break;
  case Type::TypeClass::ConstantArray:
    C.ConstantArrayTypes.RemoveNode(cast<ConstantArrayType>(T));
    break;
  case Type::TypeClass::IncompleteArray:
    C.IncompleteArrayTypes.RemoveNode(cast<IncompleteArrayType>(T));
    break;
  case Type::TypeClass::DependentSizedArray:
    C.DependentSizedArrayTypes.RemoveNode(cast<DependentSizedArrayType>(T));
    break;
  case Type::TypeClass::DependentSizedExtVector:
    C.DependentSizedExtVectorTypes.RemoveNode(
        cast<DependentSizedExtVectorType>(T));
    break;
  case Type::TypeClass::ExtVector:
  case Type::TypeClass::Vector:
    C.VectorTypes.RemoveNode(cast<VectorType>(T));
    break;
  case Type::TypeClass::FunctionNoProto:
    C.FunctionNoProtoTypes.RemoveNode(cast<FunctionNoProtoType>(T));
    break;
  case Type::TypeClass::FunctionProto:
    C.FunctionProtoTypes.RemoveNode(cast<FunctionProtoType>(T));
    break;
  case Type::TypeClass::TemplateTypeParm:
    C.TemplateTypeParmTypes.RemoveNode(cast<TemplateTypeParmType>(T));
    break;
  case Type::TypeClass::SubstTemplateTypeParm:
    C.SubstTemplateTypeParmTypes.RemoveNode(cast<SubstTemplateTypeParmType>(T));
    break;
  case Type::TypeClass::SubstTemplateTypeParmPack:
    C.SubstTemplateTypeParmPackTypes.RemoveNode(
        cast<SubstTemplateTypeParmPackType>(T));
    break;
  case Type::TypeClass::TemplateSpecialization:
    C.TemplateSpecializationTypes.RemoveNode(
        cast<TemplateSpecializationType>(T));
    break;
  case Type::TypeClass::Paren:
    C.ParenTypes.RemoveNode(cast<ParenType>(T));
    break;
  case Type::TypeClass::Elaborated:
    C.ElaboratedTypes.RemoveNode(cast<ElaboratedType>(T));
    break;
  case Type::TypeClass::DependentName:
    C.DependentNameTypes.RemoveNode(cast<DependentNameType>(T));
    break;
  case Type::TypeClass::DependentTemplateSpecialization:
    C.DependentTemplateSpecializationTypes.RemoveNode(
        cast<DependentTemplateSpecializationType>(T));
    break;
  case Type::TypeClass::PackExpansion:
    C.PackExpansionTypes.RemoveNode(cast<PackExpansionType>(T));
    break;
  case Type::TypeClass::ObjCObject:
  case Type::TypeClass::ObjCInterface:
    C.ObjCObjectTypes.RemoveNode(cast<ObjCObjectTypeImpl>(T));
    break;
  case Type::TypeClass::ObjCObjectPointer:
    C.ObjCObjectPointerTypes.RemoveNode(cast<ObjCObjectPointerType>(T));
    break;
  case Type::TypeClass::ObjCTypeParam:
    C.ObjCTypeParamTypes.RemoveNode(cast<ObjCTypeParamType>(T));
    break;
  case Type::TypeClass::Auto:
    C.AutoTypes.RemoveNode(cast<AutoType>(T));
    break;
  case Type::TypeClass::DeducedTemplateSpecialization:
    C.DeducedTemplateSpecializationTypes.RemoveNode(
        cast<DeducedTemplateSpecializationType>(T));
    break;
  case Type::TypeClass::Atomic:
    C.AtomicTypes.RemoveNode(cast<AtomicType>(T));
    break;
  case Type::TypeClass::Pipe:
    C.PipeTypes.RemoveNode(cast<PipeType>(T));
    break;
  case Type::TypeClass::Attributed:
    C.AttributedTypes.RemoveNode(cast<AttributedType>(T));
    break;
  }
}

void Unloader::eraseTypesASTContext() {
  for (const Type *cT : UD.Types) {
    Type *T = const_cast<Type *>(cT);
    C.MemoizedTypeInfo.erase(T);
    RemoveNode(C, T);
  }
  {
    auto &V = C.VariableArrayTypes;
    auto I = std::remove_if(V.begin(), V.end(),
                            [&](Type *T) { return UD.Types.count(T); });
    V.erase(I, V.end());
  }
  {
    auto &V = C.Types;
    auto I = std::remove_if(V.begin(), V.end(),
                            [&](Type *T) { return UD.Types.count(T); });
    V.erase(I, V.end());
  }
}

void Unloader::eraseNestedNameSpecifiersASTContext() {
  for (const NestedNameSpecifier *NNS : UD.NestedNameSpecifiers)
    C.NestedNameSpecifiers.RemoveNode(
        const_cast<NestedNameSpecifier *>(NNS));
}

void Unloader::eraseDeclContextsASTContext() {
// FIXME, use UD.Decls cast to and share this with FindInDeclContext
#if 0
  for (const DeclContext *DC: UD.DeclContexts)
    C.MangleNumberingContexts.erase(DC);
#endif
}

void Unloader::recanonTypes() {
  for (Type *T : UD.RecanonTypes) {
    if (UD.Types.count(T))
      continue;
    QualType QT;
    if (DecltypeType *DT = dyn_cast<DecltypeType>(T))
      QT = C.getDecltypeType(DT->getUnderlyingExpr(), DT->getUnderlyingType());
    else if (TemplateSpecializationType *TST =
                 dyn_cast<TemplateSpecializationType>(T))
      QT = C.getCanonicalTemplateSpecializationType(TST->getTemplateName(),
                                                    TST->template_arguments());
    else if (FunctionProtoType *FPT = dyn_cast<FunctionProtoType>(T))
      QT = C.getFunctionType(FPT->getReturnType(), FPT->getParamTypes(),
                             FPT->getExtProtoInfo());
    else if (DependentNameType *DNT = dyn_cast<DependentNameType>(T))
      QT = C.getDependentNameType(DNT->getKeyword(), DNT->getQualifier(),
                                  DNT->getIdentifier());
    assert(!QT.isNull());
    if (QT.getTypePtr() == T)
      T->setCanonicalTypeInternal(QT);
    else
      T->setCanonicalTypeInternal(QT.getCanonicalType());
  }
}

static void removeMacrosVector(Preprocessor &PP,
                               Preprocessor::MacrosVector &MV) {
  for (auto I : llvm::reverse(MV))
    PP.removeMacro(I.first, I.second);
}

void Unloader::eraseMacros() {
  for (const FileEntry *FE : UD.FileEntries) {
    auto J = PP.GeneratedMacros.find(FE);
    if (J != PP.GeneratedMacros.end())
      removeMacrosVector(PP, J->second);
    PP.GeneratedMacros.erase(FE);
  }
  removeMacrosVector(PP, PP.NeedMacros);
  PP.NeedMacros.clear();
  removeMacrosVector(PP, PP.GeneratedMacrosCommandLine);
  PP.GeneratedMacrosCommandLine.clear();
  for (auto &Pair : PP.CommandLineMainIdentifiersIdentifiers)
    Pair.second = macroUndefined;
}

Unloader::Unloader(Sema &S, CodeGen::CodeGenModule *Builder, UserData &UD,
                   FileDeclsMapping &FDMapping)
    : S(S), PP(S.getPreprocessor()), C(S.getASTContext()),
      SM(S.getSourceManager()), Builder(Builder), UD(UD), FDMapping(FDMapping),
      M(nullptr) {}

} // namespace clang
