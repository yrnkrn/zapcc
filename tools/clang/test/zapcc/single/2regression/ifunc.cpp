// RUN: %zapccxx -c %s -o %t  -target x86_64-pc-linux > %t.txt 2>&1
// RUN: FileCheck %s -check-prefix=CHECK-COMPILE -allow-empty < %t.txt 
// RUN: %zapccxx -c %s -o %t  -target x86_64-pc-linux > %t.txt 2>&1
// RUN: FileCheck %s -check-prefix=CHECK-COMPILE -allow-empty < %t.txt 
// CHECK-COMPILE-NOT: note: system {{.*}} was removed
// CHECK-COMPILE-NOT: error: 
// CHECK-COMPILE-NOT: warning: 
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
extern "C" void *goo_ifunc(void) { return 0; }
extern void goo(void) __attribute__((ifunc("goo_ifunc")));
