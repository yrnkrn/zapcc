#include "f.h"
void foo2() {
  Bug1104minus<int> b;
  b - b;
}
