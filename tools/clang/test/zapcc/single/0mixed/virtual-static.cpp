// RUN: %zap_compare_object
#include "virtual-static.h"
void MixedStruct::OtherFunc() {
  static MainTemplate<MixedStruct> Boom;
}
int main() { }
