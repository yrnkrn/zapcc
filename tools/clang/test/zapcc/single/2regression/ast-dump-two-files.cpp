// RUN: %zapccxx -fsyntax-only -Xclang -ast-dump %s %S/alias-attr.cpp > %t.txt 2>&1 || true
// RUN: FileCheck %s -check-prefix=CHECK-COMPILE < %t.txt
// CHECK-COMPILE-NOT: note: system {{.*}} was removed
// CHECK-COMPILE-NOT: error: 
// CHECK-COMPILE-NOT: warning: 
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
// Bug 242
