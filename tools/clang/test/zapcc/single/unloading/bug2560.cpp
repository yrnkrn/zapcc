// XFAIL: *
// REQUIRES: debug
// template declaration and definitions are merged
// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: bug2560.h U
#include "bug2560.h"
template <typename T> void Bug2560pair<T>::foo() {}
