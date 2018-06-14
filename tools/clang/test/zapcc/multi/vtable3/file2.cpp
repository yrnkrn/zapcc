#include "f.h"
void TopClass3::f2() {}
TopClass3::~TopClass3() {}
extern "C" int puts(const char *);
int main() {
  puts("vtable3");
}
