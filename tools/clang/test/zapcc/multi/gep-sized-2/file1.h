#pragma once
class Bug991SDep;
template <typename a> void Bug991b(a &c, __PTRDIFF_TYPE__ d) { c += d; }
template <typename a> void Bug991advance(a &c) {
  __PTRDIFF_TYPE__ d;
  Bug991b(c, d);
}
template <typename ff> void Bug991next(ff c) { 
  Bug991advance(c);
}
