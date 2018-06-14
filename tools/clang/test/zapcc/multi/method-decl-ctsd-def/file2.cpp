#include "table.h"
void foo2() {
  Bug1648Table<2, float> q;
  q = q;
}
