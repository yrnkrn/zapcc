// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ZAPCC_UNLOAD_SELECTOR
#define ZAPCC_UNLOAD_SELECTOR

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ManagedStatic.h"

namespace clang {
class WildRegex;
class ZapccConfig;

class UnloadSelector {
  WildRegex *MutableRegex = nullptr;
  WildRegex *DoNotZapRegex = nullptr;

public:
  ~UnloadSelector();
  void reset(ZapccConfig &ZC);
  bool isMutable(llvm::StringRef FileName);
  bool isDoNotZap(llvm::StringRef FileName);
};

extern llvm::ManagedStatic<UnloadSelector> US;
}

#endif
