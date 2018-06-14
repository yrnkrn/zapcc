// RUN: %zap_compare_object
#include "base-constructor-derived.h"
namespace BaseTDerived {
template struct BaseT<int>;
Derived::Derived() : BaseT<int>() {}
}
