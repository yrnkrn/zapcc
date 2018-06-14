#include "f.h"
void Bug413fn1();
void Bug413fn2() { Bug413fn1(); }
void Bug413foo1() { Bug413InputArray::InlineFunc(); }
