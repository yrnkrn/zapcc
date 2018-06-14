// RUN: %zap_compare_object
#include "struct-decl-member-3.h"
struct S {
  ~S() {
  }
};
struct StructWithDeclMember3::DeclMember {
  S s;
};
int main() {
  StructWithSelf3<StructWithDeclMember3::DeclMember>::f();
}
