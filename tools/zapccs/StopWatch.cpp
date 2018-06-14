// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef NDEBUG
#include "CommandLine.h"
#include "StopWatch.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <chrono>

using namespace llvm;

static cl::opt<bool, true> Time("time", cl::desc("Time individual commands"),
                                cl::Hidden, cl::ZeroOrMore,
                                cl::location(DebugTimeFlag));

StopWatch::StopWatch(const char *Name) : Name(Name) {
  StartTime = std::chrono::high_resolution_clock::now();
}

StopWatch::~StopWatch() {
  auto EndTime = std::chrono::high_resolution_clock::now();
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime -
                                                                    StartTime);
  if (DebugTimeFlag)
    llvm::errs() << Name << " = " << diff.count() << " ms\n";
}

#endif
