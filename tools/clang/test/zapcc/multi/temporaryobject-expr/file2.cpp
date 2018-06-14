#include "f.h"
extern "C" int puts(const char *s);
int main() {
  Bug393TemplatedMember a;
  puts("temporaryobject-expr");
}
