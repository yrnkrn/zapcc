// RUN: %zapcc_reset
// RUN: %zapccxx -target i686-pc-linux -S -emit-llvm %s -o %t.1
// RUN: %zapccxx -target i686-pc-linux -S -emit-llvm %s -o %t.2
// RUN: diff -u %t.1 %t.2
// Bug 1214
#include "byval-call.h"
void Bug1214tryAliasOpMatch(Bug1214ArrayRef) {}
