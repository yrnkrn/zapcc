#include "file1.h"
extern "C" int puts(const char *s);
int main() { 
  SpecializationAfterInstantiationFunc<int>();
  puts("specialization-after-instantiation-func");
}
