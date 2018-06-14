// RUN: %zap_compare_object
#include "virtual-local-struct-inheritance.h"
void StructVirtualBase::Func() {}
StructVirtualDerived SVD;
int main() { }
