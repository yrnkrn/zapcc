#include "f.h"
Bug441__mbstate_t a;
extern "C" int puts(const char *s);
int main() {
  puts("externc-include");
}
