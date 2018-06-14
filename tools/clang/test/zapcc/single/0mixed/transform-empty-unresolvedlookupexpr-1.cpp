// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1450
template <class T> void Bug1450foo1();
#include "transform-empty-unresolvedlookupexpr-1.h"
void boo1() { Bug1450h<int>(); }
