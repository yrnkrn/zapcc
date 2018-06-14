// RUN: %zapccxx-c -O1 -emit-llvm -std=c++11 -target x86_64-unknown-linux-gnu %s -o %t.o > %t 2>&1
// RUN: FileCheck %s --allow-empty -check-prefix=CHECK-COMPILE < %t
// RUN: %zapccxx-c -O1 -emit-llvm -std=c++11 -target x86_64-unknown-linux-gnu %s -o %t.o > %t 2>&1
// RUN: FileCheck %s --allow-empty -check-prefix=CHECK-COMPILE < %t
// CHECK-COMPILE-NOT: error:
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
// Bug 2131
#include "emit-typeinfo.h"
void CommandLineHelpModuleImplcreateExporter() { Bug2131(); }
