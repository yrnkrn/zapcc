// RUN: %zapccxx -fsyntax-only %s %s -target x86_64-pc-linux > %t.txt 2>&1
// RUN: FileCheck %s -check-prefix=CHECK-COMPILE -allow-empty < %t.txt
// CHECK-COMPILE-NOT: error: 
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
// Bug 1452
struct Bug1452S;
namespace {
enum Bug1452E { E0 };
}
void operator&(Bug1452E, Bug1452E);
namespace {
void operator&(Bug1452S &, Bug1452S &) { Bug1452E(E0 & E0); }
}
