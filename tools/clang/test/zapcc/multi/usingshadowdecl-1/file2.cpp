#include "f.h"
extern "C" int puts(const char *s);
int main() {
  Bug327Func<int>(); 
  puts("usingshadowdecl");
}
