#pragma once
template <class T> struct Bug1104minus {
  T x();
  template <class l>
  friend auto operator-(Bug1104minus<l> n, Bug1104minus<l>) -> decltype(n.x());
};
template <class l>
auto operator-(Bug1104minus<l> n, Bug1104minus<l>) -> decltype(n.x());
