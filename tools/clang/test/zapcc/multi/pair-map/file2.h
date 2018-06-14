#pragma once
struct Bug390basic_string {
  ~Bug390basic_string() {}
};
#include "file1.h"
struct Bug390map {
  Bug390pair<Bug390basic_string> p;
};
