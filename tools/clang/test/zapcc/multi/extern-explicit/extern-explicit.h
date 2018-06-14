#pragma once
extern "C" int puts(const char *);
template <typename T> class ete1 {
public:
  void f(const char *s) { puts(s); }
};
extern template class ete1<int>;
