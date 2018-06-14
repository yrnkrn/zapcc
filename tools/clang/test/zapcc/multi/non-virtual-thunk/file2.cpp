#include "f.h"
NVT_Class4::~NVT_Class4() {}
void NVT_f2() {  NVT_Class3 C3; C3.fn2(); }
extern "C" int puts(const char *s);
int main() {
  puts("non-virtual-thunk");
}
