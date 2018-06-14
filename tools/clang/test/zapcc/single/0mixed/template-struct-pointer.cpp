// RUN: %zap_compare_object
#include "template-struct-pointer.h"
template <typename>
class GW { public: };
void SD::foo() {
  GW<SD> W;
}
int main() {
  SD().foo();
}
