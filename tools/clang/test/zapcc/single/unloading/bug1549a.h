#pragma once
template <bool> struct Bug1549a_enable_if;
template <class> struct Bug1549a___wrap_iter {
  template <class _Tp> typename Bug1549a_enable_if<_Tp::value>::type boo();
};
template <class _Tp> Bug1549a___wrap_iter<_Tp> Bug1549a_begin2();
Bug1549a___wrap_iter<int> Bug1549a_begin1();
