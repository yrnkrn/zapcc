#pragma once
template <typename T> struct StructWithSelf4 {
  T *var;
  static void f() { StructWithSelf4(); }
};
struct StructWithDeclMember4 {
  struct DeclMember;
};
