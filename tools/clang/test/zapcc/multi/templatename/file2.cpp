#include "f.h"
void Bug1040c() {
  int a;
  Bug1040internal::ArgumentAdaptingMatcherFunc<Bug1040internal::HasParentMatcher> b;
  b(a);
}
