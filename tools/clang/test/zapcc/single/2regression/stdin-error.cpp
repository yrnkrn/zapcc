// RUN: %zapccxx -fsyntax-only -x c++ - < %s 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -allow-empty -input-file=%t.txt
// CHECK: unknown type name
// CHECK-NOT: retry
// Bug 310
blabla
