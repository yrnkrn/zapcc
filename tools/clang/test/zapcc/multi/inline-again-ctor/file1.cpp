#include "f.h"
void Bug413fn1();
void Bug413copySize() { Bug413fn1(); }
void Bug413foo1() {
  Bug413_InputArray b;
  b.getMat();
}
