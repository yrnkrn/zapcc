// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
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
