// RUN: %zap_compare_object
#include "struct-decl-member-2.h"
struct S {
  ~S() {
  }
};
struct StructWithDeclMember2::DeclMember {
  S s;
};
int main() {
  StructWithSelf2<StructWithDeclMember2::DeclMember>::f();
}
