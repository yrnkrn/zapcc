#pragma once
template <typename b> struct Bug981unique_ptr {
  static b *d;
  void foo() { d[0]; }
};
