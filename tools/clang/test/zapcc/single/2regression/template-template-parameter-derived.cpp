// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 682
template <template <typename> class Bug682Base>
class Bug682Derived : Bug682Base<int> {};
