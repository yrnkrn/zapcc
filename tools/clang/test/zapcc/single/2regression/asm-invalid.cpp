// RUN: %zapccxx %s -c 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty 
// CHECK-NOT: 0x0
// Bug 1114
asm("fatal error");
