// RUN: %zap_compare_object CXXFLAGS="-g -O"
// RUN: %zap_compare_object CXXFLAGS="-g -O"
#include "dinode-set.h"
void Bug824b() { Bug824basic_string<char> a = 0; }
