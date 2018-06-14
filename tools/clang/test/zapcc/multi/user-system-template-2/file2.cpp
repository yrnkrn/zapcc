#include "f.h"
extern "C" int puts(const char *s);
int main() {
  ust2_NS::ust2_vctr<int> v;
  v + 0;
  puts("user-system-template-2");
}
