#include "f.h"
struct Bug1400Base : Bug1400st2 {};
void foo() { Bug1400cl2<Bug1400Base>(nullptr); }
