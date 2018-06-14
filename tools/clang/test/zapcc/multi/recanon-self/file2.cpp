#include "Try.h"
void foo2() {
  Bug2464Try<int>().get<false>();
}
