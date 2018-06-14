#pragma once
struct NVT_Class1 {
  virtual void fn1() = 0;
};
struct NVT_Class2 {
  virtual void fn2() = 0;
};
struct NVT_Class3 : NVT_Class1, NVT_Class2 {
  void fn1() {}
  void fn2() {}
  ~NVT_Class3();
};
struct NVT_Class4 : NVT_Class3 {
  ~NVT_Class4();
};
