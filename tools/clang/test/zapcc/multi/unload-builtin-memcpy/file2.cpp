#include "f.h"
void copy() {
  char a;
  __builtin_memcpy(&a, &a, 0);
}
void foo2(Bug1135sockaddr_in e) {
  Bug1135sockaddr_in b;
  b = e;
}
