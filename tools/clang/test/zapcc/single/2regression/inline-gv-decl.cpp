// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 347
#include "inline-gv-decl.h"
void *Bug347aaa;
Bug347Base::~Bug347Base() { Bug347aaa = 0; }
void Bug347Derived::print() {}
extern "C" int puts(const char *s);
int main() {
  puts("inline-gv-decl");
}
