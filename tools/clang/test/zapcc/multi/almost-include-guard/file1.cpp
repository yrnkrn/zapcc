#include "file1.h"
extern "C" int puts(const char *s);
int main() {
  puts("almost-include-guard");
}
