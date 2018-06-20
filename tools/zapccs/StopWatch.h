// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ZAPCCS_STOPWATCH_H
#define ZAPCCS_STOPWATCH_H

#ifndef NDEBUG
#include <chrono>

class StopWatch {
  const char *Name;
  std::chrono::time_point<std::chrono::high_resolution_clock> StartTime;

public:
  StopWatch(const char *Name);
  ~StopWatch();
};

#else
struct StopWatch {
  StopWatch(const char *) {}
};
#endif

#endif
