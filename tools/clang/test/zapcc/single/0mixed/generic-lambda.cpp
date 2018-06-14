// RUN: %zapccxx -fsyntax-only -std=c++14 %s %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: error: 
// Bug 2562
#include "generic-lambda.h"
void foo() { 
  auto F = Bug2562values();
  F(0);
}
