#include "Try.h"
void foo1() {
  class A;
  Bug2464Try<A> t_a0;
  Bug2464Try<int> t;
}
