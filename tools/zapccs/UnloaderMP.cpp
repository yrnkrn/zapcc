// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "Unloader.h"

#include "../lib/CodeGen/CGOpenMPRuntime.h"

using namespace llvm;
namespace clang {
#if 0
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
#endif

void Unloader::eraseOpenMP(CodeGen::CGOpenMPRuntime *OpenMPRuntime) {
  OpenMPRuntime->KmpTaskTQTy = QualType();
  eraseDenseMap(OpenMPRuntime->OpenMPDefaultLocMap,
                [&](std::pair<unsigned, llvm::Value *> Pair) {
                  if (Constant *C = dyn_cast<Constant>(Pair.second))
                    if (ConstantsToEraseCompletely.count(C))
                      return true;
                  return false;
                });
  eraseDenseMap(OpenMPRuntime->OpenMPDebugLocMap,
                [&](std::pair<unsigned, llvm::Value *> Pair) {
                  if (Constant *C = dyn_cast<Constant>(Pair.second))
                    if (ConstantsToEraseCompletely.count(C))
                      return true;
                  return false;
                });
  eraseDenseMap(
      OpenMPRuntime->OpenMPLocThreadIDMap,
      [&](std::pair<llvm::Function *,
                    CodeGen::CGOpenMPRuntime::DebugLocThreadIdTy>
              Pair) { return ConstantsToEraseCompletely.count(Pair.first); });
  eraseDenseMap(OpenMPRuntime->UDRMap,
                [&](std::pair<const OMPDeclareReductionDecl *,
                              std::pair<llvm::Function *, llvm::Function *>>
                        Pair) {
                  return ConstantsToEraseCompletely.count(Pair.second.first) ||
                         ConstantsToEraseCompletely.count(Pair.second.second);
                });
  eraseDenseMap(
      OpenMPRuntime->FunctionUDRMap,
      [&](std::pair<llvm::Function *,
                    SmallVector<const OMPDeclareReductionDecl *, 4>> &Pair) {
        return ConstantsToEraseCompletely.count(Pair.first);
      });
  // OMG. What an awful data structure.
  for (auto &D :
       OpenMPRuntime->OffloadEntriesInfoManager.OffloadEntriesTargetRegion)
    for (auto &F : D.second)
      for (auto &P : F.second)
        eraseDenseMap(
            P.second,
            [&](std::pair<
                unsigned,
                CodeGen::CGOpenMPRuntime::OffloadEntriesInfoManagerTy::
                    OffloadEntryInfoTargetRegion> &Pair) {
              bool Erase =
                  ConstantsToEraseCompletely.count(Pair.second.getAddress());
              if (Erase)
                --OpenMPRuntime->OffloadEntriesInfoManager.OffloadingEntriesNum;
              return Erase;
            });
}

} // namespace clang
