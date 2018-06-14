// RUN: %zap_compare_object
#include "inherited-vtable.h"
void DerivedClassV::f() {}
int main() {
  DerivedClassV D;
  D.f();
}
