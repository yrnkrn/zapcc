// RUN: %zapccxx -fsyntax-only %s %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: error: 
// Bug 2550
#include "unload-function-decl.h"
class Bug2550file {};
void open_checked_file_dma() { Bug2550open_file_dma(); }
