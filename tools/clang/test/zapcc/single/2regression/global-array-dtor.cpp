// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "global-array-dtor.h"
struct LocalStruct {
  GlobalArrayDtor d;
};
GlobalArrayDtor::S::~S() {}
int main() {
  static LocalStruct a[1], b[1];
}
