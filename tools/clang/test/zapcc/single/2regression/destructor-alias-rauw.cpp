// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 334
#include "destructor-alias-rauw.h"
class ItemBug334 {};
extern "C" int puts(const char *);
int main() { 
  DerivedBug334<ItemBug334 *> I;
  puts("destructor-alias-rauw");
}
