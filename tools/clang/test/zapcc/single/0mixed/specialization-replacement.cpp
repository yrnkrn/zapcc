// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1595
#include "specialization-replacement.h"
namespace Bug1595llvm {
template class AnalysisManager<Loop>;
}
