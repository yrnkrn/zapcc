#include "f.h"
template <> int SpecializationAfterImplicit1<char>::value;
extern "C" int puts(const char *s);
int main() {
  puts("specialization-after-implicit-var-1");
}
