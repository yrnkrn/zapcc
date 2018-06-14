#pragma once
struct init {
  init();
};
template <typename> struct t {
  init f() { return i; }
  static init i;
};
template <typename T> init t<T>::i;
