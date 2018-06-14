// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "explicit-instantiation-2.h"
template class SymbolTableListTraits<Instruction>;
extern "C" int puts(const char *);
int main() {
  puts("explicit-instantiation-2.cpp");
}
