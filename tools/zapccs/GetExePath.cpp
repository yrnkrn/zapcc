// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "llvm/Support/FileSystem.h"

const char *GetExePath(const char *Argv0) {
  static std::string ExePath;
  if (!ExePath.size()) {
    // This just needs to be some symbol in the binary; C++ doesn't
    // allow taking the address of ::main however.
    void *MainAddr = (void *)(intptr_t)GetExePath;
    ExePath = llvm::sys::fs::getMainExecutable(Argv0, MainAddr);
  }
  return ExePath.c_str();
}
