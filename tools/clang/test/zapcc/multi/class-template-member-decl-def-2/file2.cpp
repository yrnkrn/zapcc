#include "transaction.hpp"
void foo2() {
  Bug1919smart_ref<int> s;
  *s;
}
