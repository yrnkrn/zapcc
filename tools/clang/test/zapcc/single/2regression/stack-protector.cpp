// RUN: %zapccxx %s -c -O -fstack-protector-strong 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty 
// RUN: %zapccxx %s -c -O -fstack-protector-strong 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty 
// CHECK-NOT: 0x0
// Bug 1177
#include "stack-protector.h"
void foo() {
  Bug1177CLHashMap normsCache;
  normsCache.put('a');
}
