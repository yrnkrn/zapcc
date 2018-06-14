// RUN: %zapccxx %s -c -fprofile-arcs 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -c -fprofile-arcs 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty

// RUN: %zapccxx %s -c --coverage 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -c --coverage 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty
// CHECK-NOT: 0x0
// Bug 1398
// Bug 1535
void foo() {
}
