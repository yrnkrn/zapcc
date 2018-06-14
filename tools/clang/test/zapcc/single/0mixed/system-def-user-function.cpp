// RUN: %zap_compare_object
#include "system-def-user-function.h"
void FirstFunc() { }
int main() {
  SecondFunc();
}
