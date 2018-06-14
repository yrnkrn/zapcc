#pragma once
template <int> struct Bug2464enable_if { typedef int type; };
template <class> struct Bug2464Try {
  template <int isTry> typename Bug2464enable_if<!isTry>::type get();
};
