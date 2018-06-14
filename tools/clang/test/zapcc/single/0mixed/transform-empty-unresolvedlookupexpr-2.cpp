// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 2223
namespace Bug2223Eigen {
template <typename> void generic_fast_tanh_float();
void tanh() { generic_fast_tanh_float<float>(); }
template <typename> void pset1();
}
#include "transform-empty-unresolvedlookupexpr-2.h"
