// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "eigen_carray_replicate.h"
void foo() {
  ECR_P p2;
  ECR_A p1;
  ECR_L::run(p1, p2);
}
