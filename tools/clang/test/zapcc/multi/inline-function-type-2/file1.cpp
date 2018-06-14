#include "inline-function-type-2.h"
struct IFT2_Device::Impl {
  int i;
};
int IFT2_Device::vendorID() { return p->i; }
void IFT2_fn1() { IFT2_Device().isIntel(); }

extern "C" int puts(const char *s);
int main() {
  puts("inline-function-type-2");
}
