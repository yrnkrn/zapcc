// RUN: %zapccxx -c  %s -o %t > %t.txt 2>&1
// RUN: FileCheck %s -allow-empty < %t.txt
// CHECK-NOT: _clang_call_terminate

namespace std {
void terminate();
}
struct b {};
void c() try { std::terminate(); } catch (b) {
  std::terminate();
}
