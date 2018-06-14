// RUN: %zapccxx -S %s -o %t -target x86_64-pc-linux
// RUN: FileCheck %s < %t
// RUN: %zapccxx -S %s -o %t -target x86_64-pc-linux
// RUN: FileCheck %s < %t

// RUN: %zapccxx -S -O %s -o %t -target x86_64-pc-linux
// RUN: FileCheck %s < %t
// RUN: %zapccxx -S -O %s -o %t -target x86_64-pc-linux
// RUN: FileCheck %s < %t

// RUN: %zapccxx -S -emit-llvm %s -o %t -target x86_64-pc-linux
// RUN: FileCheck %s -check-prefix=CHECK-LLVM < %t
// RUN: %zapccxx -S -emit-llvm %s -o %t -target x86_64-pc-linux
// RUN: FileCheck %s -check-prefix=CHECK-LLVM < %t

// RUN: %zapccxx -S -emit-llvm -O %s -o %t -target x86_64-pc-linux
// RUN: FileCheck %s -check-prefix=CHECK-LLVM < %t
// RUN: %zapccxx -S -emit-llvm -O %s -o %t -target x86_64-pc-linux
// RUN: FileCheck %s -check-prefix=CHECK-LLVM < %t

// CHECK: _Z3foov:
// CHECK-LLVM: define void @_Z3foov

// Bug 1923
// Bug 1931

#include "emit-assembly.h"
void foo() { Bug1923test(); }
