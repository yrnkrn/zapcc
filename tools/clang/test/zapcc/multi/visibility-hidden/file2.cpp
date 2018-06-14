#include "Pass.h"
void foo2() {
  struct : Bug2100Pass {
  } X;
}
