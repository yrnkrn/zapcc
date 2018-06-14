// RUN: %zap_compare_object
#include "foobarboo.h"
template <typename T> void fbb_foo<T>::fbb_far() {}
void fbb_boo::fbb_bar() { F.fbb_far(); }
