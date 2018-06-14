#include "f2.h"
using namespace AmbiguousCallNS;
static bool AmbiguousTrueOrFalse() {
  return true;
}
extern "C" int puts(const char *s);
int main() {
  if (AmbiguousTrueOrFalse())
    puts("ambiguous-call");
}
