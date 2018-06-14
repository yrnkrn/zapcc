// RUN: cp %s %t.h
// RUN: %zapccxx -x c++-header %t.h
// RUN: rm %t.h
// RUN: %zapccxx -fsyntax-only %s
// CHECK-NOT: note: system {{.*}} was removed
// CHECK-NOT: error:
// CHECK-NOT: warning:
// CHECK-NOT: Assertion
// CHECK-NOT: Stack dump:
// Bug 270
