// RUN: %zap_duplicate
#include "duplicate-function-template-specialization-decl.h"
template <typename T> void DuplicateFunctionSpecialization() {}
template <> void DuplicateFunctionSpecialization<int>() {}
