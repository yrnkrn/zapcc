#pragma once
template <typename> struct Helper {
  void h() {}
};
struct Self {
  void f();
  Helper<Self> g();
};
