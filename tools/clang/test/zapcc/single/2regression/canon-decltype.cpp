// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1429
template <typename T> void foo(T x) { foo<decltype(x)>; }
