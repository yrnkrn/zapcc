// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCC_CONFIG
#define ZAPCC_CONFIG

#include "llvm/ADT/StringMap.h"

#include <string>

namespace clang {

class ZapccConfig {
public:
  typedef llvm::SmallVector<std::string, 8> PatternsType;
  typedef llvm::StringMap<PatternsType> AllPatternsType;

  ZapccConfig() {}
  // return true on success.
  bool readConfigFile(llvm::StringRef ExePath);
  unsigned getMaxMemory() const { return MaxMemory; }
  PatternsType &getPatterns(llvm::StringRef Section);

private:
  AllPatternsType AllPatterns;
  unsigned MaxMemory;

  ZapccConfig(ZapccConfig &) = delete;
  void operator=(ZapccConfig &) = delete;
};
}

#endif
