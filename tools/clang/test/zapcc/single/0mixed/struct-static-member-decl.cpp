// RUN: %zap_compare_object
#include "struct-static-member-decl.h"
struct StaticMember {
  int a;
};
StaticMember StructWithStatic::ST;
int main() {
  StructWithStatic::ST.a = 1;
}
