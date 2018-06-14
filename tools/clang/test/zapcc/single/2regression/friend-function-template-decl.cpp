// RUN: %zap_compare_object
// Bug 1771
template <class> class bos {};
template <class> class iom {
  template <class T> friend void operator<<(bos<T>, iom) {}
};
void foo() { bos<char>() << iom<char>(); }
