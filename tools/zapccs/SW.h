// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "llvm/Support/raw_ostream.h"
#include <chrono>

class SW {
  const char *Name;
  std::chrono::time_point<std::chrono::high_resolution_clock> StartTime;

public:
  SW(const char *Name) : Name(Name) {
    StartTime = std::chrono::high_resolution_clock::now();
  }
  ~SW() {
    auto EndTime = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
        EndTime - StartTime);
    llvm::errs() << Name << " = " << diff.count() << " ms\n";
  }
};
