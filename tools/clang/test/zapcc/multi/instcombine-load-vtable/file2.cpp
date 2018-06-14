#include "f.h"
void foo2() {
  Bug1467cl1 *v = new Bug1467cl2;
  v->~Bug1467cl1();
}
