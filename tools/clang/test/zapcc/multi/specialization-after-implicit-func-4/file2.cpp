#include "file1.h"
extern "C" int puts(const char *s);
int main() { SpecializationAfterImplicitFunc4<int>(); }
template <> int SpecializationAfterImplicitFunc4() {
  return puts("specialization-after-implicit-func-4");
}
