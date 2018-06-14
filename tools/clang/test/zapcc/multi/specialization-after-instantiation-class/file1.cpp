#include "file1.h"
extern "C" int puts(const char *s);
void foo(SpecializationAfterInstantiationClass<int> &C) {}
int main() { 
  puts("specialization-after-instantiation-class");
}
