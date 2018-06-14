// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: bug1581c.h U
#include "bug1581c.h"
Bug1581c_regex_error::Bug1581c_regex_error() {}
