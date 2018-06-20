// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "DebugInfoScanner.h"

#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

DebugInfoScanner::DebugInfoScanner(const Module *M,
                                   SmallPtrSetImpl<const DINode *> &Visited,
                                   SmallPtrSetImpl<const DINode *> &UserDINodes)
    : Visited(Visited), UserDINodes(UserDINodes) {
#ifndef NDEBUG
  LastUser = nullptr;
  WhyThisDINode = nullptr;
#endif
}

#ifndef NDEBUG
void DebugInfoScanner::dumpDependencies(DINode *DIN) {
  llvm::errs() << "==== DebugInfo chain begin.\n";
  do {
    WhyThisDINode = DIN;
    DIN->dump();
    llvm::errs() << "==== Depends on DINode:\n";
    DIN = Dependencies[DIN];
  } while (DIN);
  llvm::errs() << "==== Chain end.\n\n";
}
#endif

bool DebugInfoScanner::process(DITemplateParameterArray DITPA) {
  for (DITemplateParameter *DITP : DITPA)
    if (process(DITP)) {
      return true;
    }
  return false;
}

bool DebugInfoScanner::process(DITypeRefArray DITRA) {
  for (DITypeRef DITR : DITRA)
    if (process(DITR))
      return true;
  return false;
}

bool DebugInfoScanner::process(DINodeArray DINA) {
  for (DINode *DIN : DINA) {
    if (process(DIN))
      return true;
  }
  return false;
}

bool DebugInfoScanner::process(const Function &F) {
  process(F.getSubprogram());
  for (auto &BB : F)
    for (auto &I : BB) {
      if (DILocation *DIL = I.getDebugLoc())
        process(DIL->getScope());
      if (const DbgDeclareInst *DDI = dyn_cast<DbgDeclareInst>(&I))
        process(DDI->getVariable());
      else if (const DbgValueInst *DVI = dyn_cast<DbgValueInst>(&I))
        process(DVI->getVariable());
    }
  return false;
}

bool DebugInfoScanner::process(const GlobalVariable &GV) {
  SmallVector<DIGlobalVariableExpression *, 4> DIs;
  GV.getDebugInfo(DIs);
  for (DIGlobalVariableExpression *DI : DIs)
    process(DI->getVariable());
  return false;
}

bool DebugInfoScanner::process(DINode *DIN) {
  if (!DIN)
    return false;
  if (UserDINodes.count(DIN)) {
#ifndef NDEBUG
    LastUser = DIN;
#endif
    return true;
  }
  if (!Visited.insert(DIN).second)
    return false;
  bool User = processHelper(DIN);
  if (User) {
    UserDINodes.insert(DIN);
#ifndef NDEBUG
    if (LastUser)
      Dependencies[DIN] = LastUser;
    LastUser = DIN;
#endif
#if 0
    if (DICompositeType *DIT = dyn_cast<DICompositeType>(DIN)) {
      if (DIT->getIdentifier() == "_ZTS13ByteQueueNode") {
        dumpDependencies(DIT);
      }
    }
#endif
  }
  return User;
}

bool DebugInfoScanner::processHelper(DINode *DIN) {
#if 0
  SmallVector<char, 256> Name;
  raw_svector_ostream OS(Name);
  DIN->print(OS);
  if (OS.str().find("MyClass") != StringRef::npos) {
    llvm::errs() << "\n\n\n";
  }
#endif
  if (DIType *DIT = dyn_cast<DIType>(DIN))
    if (process(DIT->getScope()))
      return true;
  if (DIDerivedType *DIDT = dyn_cast<DIDerivedType>(DIN)) {
    if (process(DIDT->getBaseType()))
      return true;
    if (DIDT->getTag() == dwarf::DW_TAG_ptr_to_member_type)
      if (process(DIDT->getClassType()))
        return true;
  }
  if (DICompositeType *DICT = dyn_cast<DICompositeType>(DIN)) {
    if (process(DICT->getBaseType()))
      return true;
    if (process(DICT->getElements()))
      return true;
    if (process(DICT->getVTableHolder()))
      return true;
    if (process(DICT->getTemplateParams()))
      return true;
  }
  if (DISubroutineType *DIST = dyn_cast<DISubroutineType>(DIN))
    if (process(DIST->getTypeArray()))
      return true;
  if (DISubprogram *DIS = dyn_cast<DISubprogram>(DIN)) {
    if (process(DIS->getScope()))
      return true;
    if (process(DIS->getType()))
      return true;
    if (process(DIS->getContainingType()))
      return true;
    if (process(DIS->getTemplateParams()))
      return true;
    if (process(DIS->getDeclaration()))
      return true;
    if (process(DIS->getVariables()))
      return true;
  }
  if (DILexicalBlockBase *DILBB = dyn_cast<DILexicalBlockBase>(DIN))
    if (process(DILBB->getScope()))
      return true;
  if (DINamespace *DINS = dyn_cast<DINamespace>(DIN))
    if (process(DINS->getScope()))
      return true;
  if (DIModule *DIM = dyn_cast<DIModule>(DIN))
    if (process(DIM->getScope()))
      return true;
  // DITemplateTypeParameter, DITemplateValueParameter
  if (DITemplateParameter *DITP = dyn_cast<DITemplateParameter>(DIN)) {
    if (process(DITP->getType()))
      return true;
  }
  if (DIVariable *DIV = dyn_cast<DIVariable>(DIN)) {
    if (process(DIV->getScope()))
      return true;
    if (process(DIV->getType()))
      return true;
    if (DIGlobalVariable *DIGV = dyn_cast<DIGlobalVariable>(DIN))
      if (process(DIGV->getStaticDataMemberDeclaration()))
        return true;
    if (DILocalVariable *DILV = dyn_cast<DILocalVariable>(DIN))
      if (process(DILV->getScope()))
        return true;
  }
  if (DIObjCProperty *DIOCP = dyn_cast<DIObjCProperty>(DIN))
    if (process(DIOCP->getType()))
      return true;
  if (DIImportedEntity *DIIE = dyn_cast<DIImportedEntity>(DIN)) {
    if (process(DIIE->getScope()))
      return true;
    if (process(DIIE->getEntity()))
      return true;
  }
  return false;
}
