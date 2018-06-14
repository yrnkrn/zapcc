// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1378
// Bug 1428
#include "return-type-specialization.h"
template <typename T> struct Bug1378tag<Bug1378tuple<T>> {};
void foo1() { Bug1378area(Bug1378tuple<int>()); }
