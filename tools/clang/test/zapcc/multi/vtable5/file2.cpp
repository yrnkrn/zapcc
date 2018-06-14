#include "f.h"
void DerivedClassThunk5::fff() {}
extern "C" int puts(const char *);
int main() {
  DerivedClassThunk5 D;
  puts("vtable5");
}
