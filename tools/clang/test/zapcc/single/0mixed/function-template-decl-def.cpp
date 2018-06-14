// RUN: %zap_compare_object
#include "function-template-decl-def.h"
template <typename T>T doublerImpl(T x) { return x + x; }
int main() {
  char Jnull[] = {doubler(32), 0};
}
