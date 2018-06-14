// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef LLVM_IR_DEBUGINFOSCANNER_H
#define LLVM_IR_DEBUGINFOSCANNER_H

#include "llvm/IR/DebugInfo.h"

namespace llvm {

template <typename> class SmallPtrSetImpl;

class DebugInfoScanner {
  SmallPtrSetImpl<const DINode *> &Visited;
  SmallPtrSetImpl<const DINode *> &UserDINodes;
  template <typename T> bool process(TypedDINodeRef<T> TDNR) {
    return process(TDNR.resolve());
  }
  bool process(DITemplateParameterArray DITPA);
  bool process(DITypeRefArray DITRA);
  bool process(DINodeArray DINA);
  bool processHelper(DINode *DIN);
#ifndef NDEBUG
  DINode *LastUser;
  DenseMap<DINode *, DINode *> Dependencies;
  void dumpDependencies(DINode *DIN);
#endif

public:
  DebugInfoScanner(const Module *M, SmallPtrSetImpl<const DINode *> &Visited,
                   SmallPtrSetImpl<const DINode *> &UserDINodes);
  bool process(const Function &F);
  bool process(const GlobalVariable &GV);
  bool process(DINode *DIN);
  DINode *resolve(DINodeRef DNR) { return DNR.resolve(); }
#ifndef NDEBUG
  DINode *WhyThisDINode;
#endif
};

} // end namespace llvm

#endif
