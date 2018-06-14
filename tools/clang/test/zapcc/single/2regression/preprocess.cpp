// RUN: %zapccxx -E -x c++ %s | tee %t && FileCheck %s -input-file=%t
// CHECK-NOT: note: system {{.*}} was removed
// CHECK-NOT: error: 
// CHECK-NOT: warning: 
// CHECK: "<built-in>"
