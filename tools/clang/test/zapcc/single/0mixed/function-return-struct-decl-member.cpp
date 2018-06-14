// RUN: %zap_compare_object
#include "function-return-struct-decl-member.h"
class ClassDeclaredInHeader {};
StructWithTypenameOnly<ClassDeclaredInHeader> FunctionReturningTemplated() {
  return StructWithTypenameOnly<ClassDeclaredInHeader>();
}
int main() { FunctionReturningTemplated(); }
