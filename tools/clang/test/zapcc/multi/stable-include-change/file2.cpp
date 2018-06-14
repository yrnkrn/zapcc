#include "trivial.h"
#include "keyword.h"
extern "C" int puts(const char *s);
int main() {
  StableIncludeChangeTag t;
  puts("stable-include-change");
}
