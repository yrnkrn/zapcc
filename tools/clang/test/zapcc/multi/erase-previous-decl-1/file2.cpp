#include "f.h"
void Bug304Fn() {};
template <int POLY> struct Bug304Base<POLY>::Implementation {
  void m_fn1() { Bug304Fn(); }
};
template class Bug304Base<0>;
