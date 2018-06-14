// RUN: %zap_compare_object CXXFLAGS="-O"
// RUN: %zap_compare_object CXXFLAGS="-O"
class Bug889fwd {};
#include "namespace-friend-1.h"
#include "namespace-friend-2.h"
