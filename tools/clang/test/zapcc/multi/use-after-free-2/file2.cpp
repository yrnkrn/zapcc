#include "f.h"
extern "C" int puts(const char *s);
int main() {
  puts("use-after-free-2");
  return UAF2Struct::getUAF();  
}
int UAF2[1] = {0};
