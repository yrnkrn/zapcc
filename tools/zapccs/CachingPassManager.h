// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef LLVM_IR_CACHINGPASSMANAGER_H
#define LLVM_IR_CACHINGPASSMANAGER_H

#include "clang/CodeGen/BackendUtil.h"
#include <memory>

namespace clang {
class CompilerInstance;
class DependencyMap;
}

namespace llvm {
class raw_pwrite_stream;

class CachingPassManager {
public:
  explicit CachingPassManager(clang::CompilerInstance *CI,
                              clang::DependencyMap *DM);
  ~CachingPassManager();
  void emitObj(clang::BackendAction Action, raw_pwrite_stream *AsmOutStream);

private:
  class Implementation;
  std::unique_ptr<Implementation> Impl;
  CachingPassManager(CachingPassManager &) = delete;
  CachingPassManager &operator=(CachingPassManager &) = delete;
};

} // End llvm namespace

#endif
