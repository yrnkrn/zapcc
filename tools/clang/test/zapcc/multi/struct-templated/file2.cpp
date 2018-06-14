#include "f.h"
template <class T> void StructTemplatedStruct::Visit() {}
extern "C" int puts(const char *s);
int main() {
  puts("struct-templated");
  StructTemplatedStruct s;
  s.callTemplate();
}
