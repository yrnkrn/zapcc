// RUN: %zapcc_reset
// RUN: %zapccxx %s -fsyntax-only -fwritable-strings 2>&1 | tee %t.txt && FileCheck %s -check-prefix=CHECK-WITH-WRITEABLE-STRINGS -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -fsyntax-only 2>&1 | tee %t.txt && FileCheck %s -check-prefix=CHECK-WITHOUT-WRITEABLE-STRINGS -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -fsyntax-only -fwritable-strings 2>&1 | tee %t.txt && FileCheck %s -check-prefix=CHECK-WITH-WRITEABLE-STRINGS -input-file=%t.txt --allow-empty

// CHECK-WITHOUT-WRITEABLE-STRINGS-NOT: error:
// CHECK-WITHOUT-WRITEABLE-STRINGS-NOT: changed
// CHECK-WITHOUT-WRITEABLE-STRINGS: warning: conversion from string literal
// CHECK-WITH-WRITEABLE-STRINGS-NOT: error:
// CHECK-WITH-WRITEABLE-STRINGS-NOT: changed
// CHECK-WITH-WRITEABLE-STRINGS-NOT: warning:

// RUN: %zapccxx %s -fsyntax-only 2>&1 | tee %t.txt && FileCheck %s -check-prefix=CHECK-WITH-DEPRECATED -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -fsyntax-only -Wno-deprecated 2>&1 | tee %t.txt ||true && FileCheck %s -check-prefix=CHECK-WITHOUT-DEPRECATED -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -fsyntax-only -Wdeprecated 2>&1 | tee %t.txt && FileCheck %s -check-prefix=CHECK-WITH-DEPRECATED -input-file=%t.txt --allow-empty

// CHECK-WITH-DEPRECATED-NOT: error:
// CHECK-WITH-DEPRECATED-NOT: changed
// CHECK-WITHOUT-DEPRECATED: error: __DEPRECATED
// CHECK-WITHOUT-DEPRECATED-NOT: changed

// RUN: %zapccxx %s -fsyntax-only 2>&1 | tee %t.txt && FileCheck %s -check-prefix=CHECK-WITH-EXCEPTIONS -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -fsyntax-only -fno-exceptions 2>&1 | tee %t.txt ||true && FileCheck %s -check-prefix=CHECK-WITHOUT-EXCEPTIONS -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -fsyntax-only -fexceptions 2>&1 | tee %t.txt && FileCheck %s -check-prefix=CHECK-WITH-EXCEPTIONS -input-file=%t.txt --allow-empty

// CHECK-WITH-EXCEPTIONS-NOT: error:
// CHECK-WITH-EXCEPTIONS-NOT: changed
// CHECK-WITHOUT-EXCEPTIONS: error: __cpp_exceptions
// CHECK-WITHOUT-EXCEPTIONS-NOT: changed

void CheckFOptions() {
  char *s = "write me";
  *s = 'W';
}

#ifndef __DEPRECATED
#error __DEPRECATED
#endif

#ifndef __cpp_exceptions
#error __cpp_exceptions
#endif

// RUN: not %zapccxx %s -std=c+11 -fsyntax-only 2>&1 | tee %t.txt && FileCheck %s -check-prefix=CHECK-STD-CX11 -input-file=%t.txt --allow-empty
// CHECK-STD-CX11: error: invalid value 'c+11' in '-std=c+11'
// Bug 165
