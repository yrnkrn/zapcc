#include "f.h"
void Bug385Fn() {}
extern "C" int puts(const char *s);
int main() {
  puts("clang-terminate-live");
}
