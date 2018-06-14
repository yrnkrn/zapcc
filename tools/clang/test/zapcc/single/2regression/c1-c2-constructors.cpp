// RUN: %zapccxx -c  %s -o %t > %t.txt 2>&1
// RUN: FileCheck %s -check-prefix=CHECK-COMPILE -allow-empty < %t.txt
// RUN: nm %t | FileCheck %s
// RUN: %zapccxx -c  %s -o %t > %t.txt 2>&1
// RUN: FileCheck %s -check-prefix=CHECK-COMPILE -allow-empty < %t.txt
// RUN: nm %t | FileCheck %s
// CHECK-COMPILE-NOT: error: 
// CHECK-COMPILE-NOT: warning: 
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
// CHECK: _ZN16C1C2ConstructorsC1Ev
// CHECK: _ZN16C1C2ConstructorsC2Ev

#include "c1-c2-constructors.h"
C1C2Constructors::C1C2Constructors() {}
