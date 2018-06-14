#pragma once
namespace Bug930std2 {
template <typename> struct c { template <typename> friend class c; };
template <typename a> int ag() {
  c<a> b;
  return sizeof(a);
}
}
