// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "explicit-instantiation-func.h"
template int explicit_instantiation_func<int>(int);
struct Int {
  Int(int i) : i(i) {}
  int i;
  Int operator*(Int I) { return i * I.i; }
};
template Int explicit_instantiation_func<Int>(Int);
int main() {
  explicit_instantiation_func(7);
  Int I(8);
  explicit_instantiation_func(I).i;
}
