#include "file1.h"
#include "file2.h"
struct f {
  f() { Bug1014smart_ref<int> s; }
};
