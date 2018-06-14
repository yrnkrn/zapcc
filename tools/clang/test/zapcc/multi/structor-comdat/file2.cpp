#include "f.h"
struct StructorComdatDerived : StructorComdatBase {
  void foo() override;
};
void StructorComdatDerived::foo()  {}
