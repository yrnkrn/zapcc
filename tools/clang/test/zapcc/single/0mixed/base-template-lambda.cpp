// RUN: %zap_compare_object
#include "base-template-lambda.h"
template <typename F> future async(F) {
  return future();
}
int main() {
  async([] {});
}
