#pragma once
#include "file2.h"
struct File1097_a {
  static void b() { throw File1097_SystemException(); }
};
void File1097_foo() { File1097_a::b(); }
