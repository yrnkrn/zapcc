#pragma once
#include "empty.h"
struct Bug830b {
  float a[1];
  void e() { *this = *this; }
};
