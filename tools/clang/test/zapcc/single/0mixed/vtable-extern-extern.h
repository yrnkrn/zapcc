#pragma once
extern int VEE_ExternInt;
struct VEE_baseclass {
  virtual void f() { VEE_ExternInt--; }
};
struct VEE_derivedclass : VEE_baseclass {
  VEE_derivedclass();
};
