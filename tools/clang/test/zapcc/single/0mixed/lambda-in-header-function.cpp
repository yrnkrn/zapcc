// RUN: %zap_compare_object
#include "lambda-in-header-function.h"
struct Y {};
int main() {
  LambdaInHeaderFunction<Y>();
}
