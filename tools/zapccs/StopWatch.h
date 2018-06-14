// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
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
