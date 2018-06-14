#include "f.h"
template <>
void SpecializationAfterImplicitFunc1<char>::foo() {}
int main() {
  SpecializationAfterImplicitFunc1<char>::foo();
}
