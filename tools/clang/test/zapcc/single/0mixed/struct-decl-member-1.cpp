// RUN: %zap_compare_object
#include "struct-decl-member-1.h"
struct DeclStruct::DeclMember {};
int main() {
  WrapT<DeclStruct::DeclMember> s;
}
