// RUN: %zapcc_reset
// RUN: %zapccxx -c %s -o %t -target x86_64-pc-linux -O -mllvm -print-after-all &> %t.txt
// RUN: FileCheck %s -check-prefix=CHECK-YES < %t.txt
// CHECK-YES: call void @_Unwind_Resume

// RUN: %zapccxx -c %s -o %t -target x86_64-pc-linux -O -mllvm -print-after-all &> %t.txt
// RUN: FileCheck %s < %t.txt
// CHECK-NOT: call void @_Unwind_Resume

// Bug 1884
#include "undo-dwarfehprepare.h"
