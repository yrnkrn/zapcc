#pragma once
extern "C" int puts(const char *s);
template <typename> class DD {
public:
  void f() { puts("explicit-extern.cpp"); }
};
struct MM;
extern template class DD<MM>;
