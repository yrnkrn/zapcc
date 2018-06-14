// RUN: %zapccxx -c %s -o %T/noinline-and-alwaysinline.o > %t 2>&1 || true
// RUN: FileCheck %s --allow-empty -check-prefix=CHECK-COMPILE < %t
// CHECK-COMPILE-NOT: note: system {{.*}} was removed
// CHECK-COMPILE-NOT: error:
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
// CHECK-COMPILE-NOT: Attributes 'noinline and alwaysinline' are incompatible
struct H {
  ~H() {}
};
__attribute__((always_inline)) void foo() {
  H a, b;
}
