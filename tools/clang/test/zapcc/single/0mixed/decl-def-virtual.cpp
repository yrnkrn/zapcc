// RUN: %zap_compare_object
#include "decl-def-virtual.h"
void BaseDDV::f() {}
int main() {
  DerivedDDV D;
}
