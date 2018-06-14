// RUN: %zap_compare_object CXXFLAGS="-O -w"
// RUN: %zap_compare_object CXXFLAGS="-O -w"
// RUN: %zap_compare_object CXXFLAGS="-O -w"
// Bug 2002
#include "optimize-once-1.h"
void ColouriseCoffeeScriptDoc() { Bug2002CharacterSet(""); }
