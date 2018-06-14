// RUN: %zapccxx -fsyntax-only %s %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: error: 
#include "overload-expr.h"
void OverloadExpr_g(int) {
}
void OverloadExpr2_g(int) {
}
int main() {
  OverloadExpr_call_g<int>();
  OverloadExpr2_call_g<int>();
}
