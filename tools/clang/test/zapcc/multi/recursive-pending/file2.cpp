#include "recursive-pending.h"
template <class T> void RecursivePending<T>::foo() {}

void fn1() {
  RecursivePending<int> v;
  v.foo();
}
