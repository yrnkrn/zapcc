// RUN: %zapccxx -target i686-pc-windows-gnu -S -emit-llvm %s -o %t
// RUN: FileCheck %s < %t
// RUN: %zapccxx -target i686-pc-windows-gnu -S -emit-llvm %s -o %t
// RUN: FileCheck %s < %t

// CHECK-COMPILE-NOT: note: system {{.*}} was removed
// CHECK-COMPILE-NOT: error:
// CHECK-COMPILE-NOT: warning:
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
// CHECK: "target-features"

int main() {}
