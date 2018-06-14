#pragma once
#include "file2.h"
struct Bug1029FastMutex {
  void lock();
};
void Bug1029FastMutex::lock() { Bug1029SystemException(); }
