// RUN: %zap_duplicate
// Bug 265
#include "duplicate-mangled-name.h"
template <int> struct placeholder {};
int main() {
  expr<placeholder<0>> e;
  e + 0;
}
