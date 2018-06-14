template <typename _Tp> struct Bug1270map {
  Bug1270map() { _Tp(); }
};
struct Bug1270Property {
  Bug1270Property();
};
void foo1() { Bug1270map<Bug1270Property> Properties; }
template <typename> int Bug1270forward;
#include "alloc_traits.h"
template <typename> struct Bug1270_List_base {
  typedef Bug1270__alloc_traits<char> _Node_alloc_type;
};
