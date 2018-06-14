// RUN: %zapccxx -g -c -emit-llvm %S/meta-pow3-bc.cpp -o %T/meta-pow3-bc.bc -O2 > %t 2>&1
// RUN: FileCheck %s --allow-empty -check-prefix=CHECK-COMPILE < %t
// RUN: %zapccxx -g -c -emit-llvm %S/meta-pow3-bc.cpp -o %T/meta-pow3-bc.bc -O2 > %t 2>&1
// RUN: FileCheck %s --allow-empty -check-prefix=CHECK-COMPILE < %t

// CHECK-COMPILE-NOT: note: system {{.*}} was removed
// CHECK-COMPILE-NOT: error:
// CHECK-COMPILE-NOT: warning:
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:

template <int N>
class Pow3 {
public:
  enum { result = 3 * Pow3<N - 1>::result };
};
template <>
class Pow3<0> {
public:
  enum { result = 1 };
};
