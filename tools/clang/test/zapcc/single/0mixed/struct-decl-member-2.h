#pragma once
template <typename T> struct StructWithSelf2 {
  T var;
  static void f() { StructWithSelf2(); }
};
struct StructWithDeclMember2 {
  struct DeclMember;
};
