#pragma once
struct Bug637_GlobalVariable;
template <typename SubClass> int *Bug637_op_begin(SubClass *p1) {
  return reinterpret_cast<int *>(p1) - 1;
}
struct Bug637_User {
  static int *OpFrom(Bug637_GlobalVariable *p1) { return Bug637_op_begin(p1); }
};
