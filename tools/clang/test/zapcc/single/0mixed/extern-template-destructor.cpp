// RUN: %zap_compare_object CXXFLAGS="-O"
// RUN: %zap_compare_object CXXFLAGS="-O"
#include "extern-template-destructor.h"
Bug2547basic_string<char> foo;
