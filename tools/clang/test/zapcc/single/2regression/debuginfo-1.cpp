// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "debuginfo-1.h"
struct myClass {};
int main() {
  DI_priority_queue<int> p1;
  DI_priority_queue<myClass> p2;
}
