// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 384
#include "missing-vtable-1.h"
int main() {
  aa a;
  a.foo();
}
