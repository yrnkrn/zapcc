// RUN: %zap_compare_object
#include "return-struct.h"
extern "C" int puts(const char *s);
struct ReturnStruct2 {};
ReturnStruct2 FunctionReturningStruct2() {
  puts("return-struct");
  return ReturnStruct2();
}

int main() {
  (void)FunctionReturningStruct2();
}
