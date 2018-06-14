// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
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
