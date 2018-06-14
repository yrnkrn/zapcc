#include "f.h"
int uaf2() { return UAF::getS(); }
UAFStructDerived UAF::StaticUAF;
extern "C" int puts(const char *s);
int main() {
  puts("use-after-free-1");
}
