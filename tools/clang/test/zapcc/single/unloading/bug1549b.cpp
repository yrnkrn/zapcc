// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: bug1549b.h U
#include "bug1549b.h"
struct Bug1549b_FileFn;
Bug1549b___wrap_iter<Bug1549b_FileFn> begin() { Bug1549b_begin1(); }
