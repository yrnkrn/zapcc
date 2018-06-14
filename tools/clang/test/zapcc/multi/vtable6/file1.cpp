#include "f.h"
void userFunc();
void V6headerFunc() { userFunc(); }
extern "C" int puts(const char *);
int main() {
  puts("vtable6");
}
