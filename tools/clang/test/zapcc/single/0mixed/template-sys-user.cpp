// RUN: %zap_compare_object
#include "template-sys-user.h"
template <typename T> ttsu_foo<T>::ttsu_foo() {}
ttsu_foo<int> a;
int main() { }
