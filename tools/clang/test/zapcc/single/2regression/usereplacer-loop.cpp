// RUN: %zapccxx %s -c 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty
// RUN: %zapccxx %s -c 2>&1 | tee %t.txt || true
// RUN: FileCheck %s -input-file=%t.txt --allow-empty
// CHECK-NOT: definition with same mangled name as another definition
// Bug 1414
#include "usereplacer-loop.h"
template <class> struct Bug1414any {
  struct type {
    typedef int node_ptr;
  };
};
void foo1() {
Bug1414iiterator_members<Bug1414any<int>::type::node_ptr, Bug1414any<int>::type> x;
}
