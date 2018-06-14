#pragma once
template <typename _Tp> struct Bug1172integral_constant { static _Tp value; };
template <typename _Tp> _Tp Bug1172integral_constant<_Tp>::value;
namespace Bug1172mpl_ {}
#include "adl_barrier.hpp"
namespace Bug1172mpl_ {
void assert_not_arg() { Bug1172mpl::assert_not_arg; }
}
