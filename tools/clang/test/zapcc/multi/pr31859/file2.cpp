#include "f.h"
template <class A> void foo() {
  Bug1858vec_traits<A>::template write_element<0>;
}
void test() { foo<Bug1858col_<int>>(); }
