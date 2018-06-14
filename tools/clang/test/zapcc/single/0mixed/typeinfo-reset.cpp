// RUN: %zap_compare_object
// Bug 432
#include "typeinfo-reset.h"
struct Bug432B : Bug432A {};
struct Bug432C : Bug432B {
  void foo(); 
};
void Bug432C::foo() { }
int main() {
}
