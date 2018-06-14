// RUN: %zapccxx -fsyntax-only -I %S %s -target x86_64-pc-linux > %t.txt 2>&1
// RUN: FileCheck %s -check-prefix=CHECK-COMPILE -allow-empty < %t.txt
// CHECK-COMPILE-NOT: error: 
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump: 
#define KWSYS_HEADER0(x) <x>
#include KWSYS_HEADER0(include-macro-1.h)
