#pragma once
extern "C" int puts(const char *);
template <typename T> class ete2 {
public:
  constexpr int f() const { return 0; }
};
extern template class ete2<int>;
