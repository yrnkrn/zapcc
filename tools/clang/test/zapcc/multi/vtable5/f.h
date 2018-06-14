#pragma once
struct BaseClassThunk5 {
  int i;
  virtual void fff() {}
  virtual void ggg() {}
};
struct DerivedClassThunk5 : virtual BaseClassThunk5 {
  void fff();
  void ggg();
};
