#include "inline-function-type-1.h"
void IFT1_foo() {
  IFT1_ByteQueue member;
  while (true)
    if (member.AnyRetrievable())
      return;
}
