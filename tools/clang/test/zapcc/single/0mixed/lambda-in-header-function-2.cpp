// RUN: %zap_compare_object
#include "lambda-in-header-function-2.h"
int main() {
  LambdaInHeaderFunction3([] {});
}
