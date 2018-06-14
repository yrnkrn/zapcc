// RUN: %zapccxx -g -c -target i686-pc-windows-gnu -S -emit-llvm %S/meta-pow3-ll.cpp -o %T/meta-pow3-ll.ll -O2 > %t 2>&1
// RUN: FileCheck %s --allow-empty -check-prefix=CHECK-COMPILE < %t
// RUN: FileCheck %s < %T/meta-pow3-ll.ll
// RUN: %zapccxx -g -c -target i686-pc-windows-gnu -S -emit-llvm %S/meta-pow3-ll.cpp -o %T/meta-pow3-ll.ll -O2 > %t 2>&1
// RUN: FileCheck %s --allow-empty -check-prefix=CHECK-COMPILE < %t
// RUN: FileCheck %s < %T/meta-pow3-ll.ll

// CHECK-COMPILE-NOT: note: system {{.*}} was removed
// CHECK-COMPILE-NOT: error:
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
// CHECK: target triple = "i686-pc-windows-gnu"
// CHECK: define i32 @main()
// CHECK: !llvm.dbg.cu = !{!0}

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
int main() {
  Pow3<4>::result;
}
