#pragma once
class Bug611Argument;
class Bug611Function;
template <class> struct Bug611A {
  static int *op_begin(Bug611Function *p1) { return reinterpret_cast<int *>(p1); }
};
struct Bug611Constant {
  int i;
  int *OpFrom(Bug611Function *p1) { return Bug611A<Bug611Function>::op_begin(p1); }
};
struct Bug611Function : Bug611Constant {
  void setPersonalityFn();
  Bug611A<Bug611Argument> getSublistAccess0;
};
