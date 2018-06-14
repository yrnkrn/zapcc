// RUN: %zap_compare_object
#include "virtual-local-struct.h"
struct LocalStruct {
  void f() { }
} LocalStruct;
void StructVirtualFunc::Func() { 
  LocalStruct.f();
}
int main() {
}
