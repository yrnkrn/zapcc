// RUN: %zapccxx -c  %s -o %t > %t.txt 2>&1
// RUN: FileCheck %s -allow-empty < %t.txt
// CHECK-NOT: was removed
// Bug 1024
#include "vtable-depends-destructor.h"
Bug1024Class::~Bug1024Class() {}
