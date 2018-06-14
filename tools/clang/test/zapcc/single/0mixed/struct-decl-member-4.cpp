// RUN: %zap_compare_object
#include "struct-decl-member-4.h"
struct S {
  ~S() {}
};
struct StructWithDeclMember4::DeclMember {
  S s;
};
int main() {
  StructWithSelf4<StructWithDeclMember4::DeclMember>::f();
}
