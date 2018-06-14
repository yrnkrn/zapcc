// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: bug1549a.h U
#include "bug1549a.h"
struct Bug1549a_FileFn;
void foo() {
  Bug1549a_begin2<Bug1549a_FileFn>();
  Bug1549a_begin1();
}
