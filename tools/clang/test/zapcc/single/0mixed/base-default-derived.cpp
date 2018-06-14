// RUN: %zap_compare_object
#include "base-default-derived.h"
template class default_Base<int>;
default_Derived::default_Derived() {}
