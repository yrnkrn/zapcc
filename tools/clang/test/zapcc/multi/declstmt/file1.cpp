#include "f.h"
struct Bug917hh {};
struct Bug917A {
  char s;
  void m_fn1() { Bug917t(&s); }
};
Bug917ak<Bug917hh> fn1_an;
