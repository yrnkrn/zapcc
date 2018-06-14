#include "f.h"
extern "C" int puts(const char *);
int main() {
  UnresolvedLookupExprNS2::bar<UnresolvedLookupExprNS1::S>();
  puts("unresolved-lookup-expr");
}
void operator+(int, UnresolvedLookupExprNS1::S){};
