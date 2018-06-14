#pragma once
template <class> class Bug1165function;
template <class a> void Bug1165b(a);
template <class e> struct Bug1165function<e()> {
  template <class a> Bug1165function(a d) { Bug1165b(d); }
};
