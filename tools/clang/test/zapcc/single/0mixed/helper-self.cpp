// RUN: %zap_compare_object
#include "helper-self.h"
void Self::f() { g().h(); }
Helper<Self> Self::g() { return Helper<Self>(); }
template class Helper<Self>;
