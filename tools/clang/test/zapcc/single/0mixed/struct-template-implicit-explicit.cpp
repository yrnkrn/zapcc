// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "struct-template-implicit-explicit.h"
template class TemplatedStructA<int>;
template class TemplatedStructB<int>;
