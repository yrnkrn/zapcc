// XFAIL: *
// REQUIRES: debug
// because template declaration and definitions are merged
// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: bug1581d.h U
#include "bug1581d.h"
template <typename> void Bug1581d_AtomizeChars() {}
