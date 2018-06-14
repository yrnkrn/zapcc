#include "recursive-pending.h"
template <class T> void Helper(T v) { v.foo(); }

void fn2() {
  RecursivePending<int> v;
  Helper(v);
}
