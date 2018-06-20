// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "GetMemoryUsage.h"

// http://nadeausoftware.com/articles/2012/07/c_c_tip_how_get_process_resident_set_size_physical_memory_use

#ifdef _WIN32
#include <windows.h>
#define PSAPI_VERSION 1
#include <psapi.h>

unsigned getMemoryUsage() {
  PROCESS_MEMORY_COUNTERS MemCounters;
  if (!GetProcessMemoryInfo(GetCurrentProcess(), &MemCounters,
                            sizeof(MemCounters)))
    return 0;
  return MemCounters.WorkingSetSize >> 20;
}

#else // _WIN32
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

unsigned getMemoryUsage() {
  FILE *fp = fopen("/proc/self/statm", "r");
  if (!fp)
    return (size_t)0L;
  long rss = 0;
  if (fscanf(fp, "%*s%ld", &rss) != 1) {
    fclose(fp);
    return 0;
  }
  fclose(fp);
  return ((size_t)rss * (size_t)sysconf(_SC_PAGESIZE)) >> 20;
}

#endif // _WIN32
