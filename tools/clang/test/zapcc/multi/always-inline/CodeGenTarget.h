#pragma once
template <class> struct Bug2005basic_string {
  __attribute__((__always_inline__)) Bug2005basic_string() {}
};
extern template class Bug2005basic_string<char>;
void Bug2005foo2();
