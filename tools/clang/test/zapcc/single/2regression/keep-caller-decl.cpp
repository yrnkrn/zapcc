// RUN: %zapccxx -c  %s -O -target x86_64-pc-linux -o %t > %t.txt 2>&1
// RUN: FileCheck %s -check-prefix=CHECK-COMPILE -allow-empty < %t.txt
// CHECK-COMPILE-NOT: error: 
// CHECK-COMPILE-NOT: warning: 
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
// CHECK-COMPILE-NOT: user of
// Bug 549
#include "keep-caller-decl.h"
void Bug549fn2() {
  Bug549fn1();
  Bug549HashingByteStreamer();
}
