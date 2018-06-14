// RUN: %zapccxx -fsyntax-only %s -Xclang -x -Xclang c++
// RUN: %zapccxx -fsyntax-only %s -Xclang -x -Xclang c++-cpp-output %S | FileCheck %s --allow-empty 
// CHECK-NOT: error:
// CHECK-NOT: warning:
// CHECK-NOT: compilation arguments changed
// Bug 2246
