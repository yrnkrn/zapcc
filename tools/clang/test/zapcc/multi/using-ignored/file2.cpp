#include "file2.h"
#include "file1.h"
extern "C" int puts(const char *s);
int main() {
  UsingIgnoredFunc(0);
  puts("using-ignored");
}
