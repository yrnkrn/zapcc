// RUN: %zapccxx -c  %s -o %t
// RUN: nm %t | c++filt | FileCheck %s
// RUN: %zapccxx -c  %s -o %t
// RUN: nm %t | c++filt | FileCheck %s
// CHECK: guard variable for f()::s

#include <string>
void f() { static std::string s = "this is a string"; }
