// RUN: %zap_compare_object CXXFLAGS="-fsanitize=address"
// Bug 1070
template <typename m> struct p {
  virtual ~p() {}
  p(int) { m ad(ad); }
};
void foo1() {
  auto f = [] {};
  p<decltype(f)> a(0);
}
