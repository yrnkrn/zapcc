// RUN: %zap_compare_object
#include "vtable-extern-extern.h"
VEE_derivedclass::VEE_derivedclass() {}
extern int VEE_ExternInt;
int main() {
  return VEE_ExternInt;
}
int VEE_ExternInt = 0;
