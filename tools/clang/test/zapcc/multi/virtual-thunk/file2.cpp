#include "f.h"
DerivedThunk::~DerivedThunk() {}
extern "C" int puts(const char *s);
int main() {
  puts("virtual-thunk");
}
