// RUN: %zapccxx -fsyntax-only %s %s > %t.txt 2>&1
// RUN: FileCheck %s -allow-empty < %t.txt 
// CHECK-NOT: error: 
// Bug 1027
struct poly_function {
template <typename> struct Bug1027result;
#include "depends-cxxrecord-member.h"
};
struct F : poly_function::template Bug1027result<poly_function> {};
