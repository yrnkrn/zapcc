// RUN: %zap_compare_object
// Bug 1764
template <typename> struct Bug1764tuple { Bug1764tuple(Bug1764tuple &) = default; };
void foo() { Bug1764tuple<int> a(a); }
