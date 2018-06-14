#include "f.h"
void Bug1385foo1() {
  struct {
    char u8[];
  } s;
  Bug1385read(s.u8);
}
