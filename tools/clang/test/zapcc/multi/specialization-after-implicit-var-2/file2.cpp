#include "f.h"
template <> int SpecializationAfterImplicitVar2<char>::value = 0;
extern "C" int puts(const char *s);
int main() {
  puts("specialization-after-implicit-var-2");
}
