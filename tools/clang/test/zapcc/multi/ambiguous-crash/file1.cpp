#include "f.h"
extern "C" int puts(const char *s);
int main() {
  puts("ambiguous-crash");
}
