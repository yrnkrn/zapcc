#pragma once
template <typename T> struct StructWithSelf3 {
  T var;
  static void f() { StructWithSelf3(); }
};
struct StructWithDeclMember3 {
  struct DeclMember;
};
