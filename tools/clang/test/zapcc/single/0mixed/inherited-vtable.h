#pragma once
struct BaseClassV  {
  int i;
  virtual void f() {}
};
struct DerivedClassV : virtual BaseClassV {
  void f();
};
