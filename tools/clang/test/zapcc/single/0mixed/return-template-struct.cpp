// RUN: %zap_compare_object
#include "return-template-struct.h"
template <typename T>struct ReturnStruct1 {
  void f(T) {}
};
class ReturnStructTypeName {};
ReturnStruct1<ReturnStructTypeName> FunctionReturningStruct() {
  return ReturnStruct1<ReturnStructTypeName>();
}

int main() {
  (void)FunctionReturningStruct();
}
