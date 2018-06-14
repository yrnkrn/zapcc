// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1909
#include "class-template-user-spec.h"
template class Bug1909FullMatrix<double>;
template void Bug1909FullMatrix<double>::fill(Bug1909FullMatrix);
