#include "f.h"
void __attribute__((noinline)) Bug431fn3() {}
void __attribute__((always_inline)) Bug431fn2() { Bug431fn3(); }
