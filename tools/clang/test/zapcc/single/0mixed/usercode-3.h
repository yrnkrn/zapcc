#pragma once
template <typename T> struct usercode3 {
  T *imp;
  void print() { imp->print();}
};
