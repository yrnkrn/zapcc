// RUN: %zap_compare_object CXXFLAGS="-O"
// RUN: %zap_compare_object CXXFLAGS="-O"
// Bug 1006
template <typename> struct a {
  ~a() {}
};
extern template struct a<char>;
template <typename T> struct b {
  struct : a<T> {};
  ~b() {
    a<T> x = a<T>();
  }
};
void c() { b<char> d; }
