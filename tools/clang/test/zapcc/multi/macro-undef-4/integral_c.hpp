#ifndef Bug1200BOOST_MPL_INTEGRAL_C_HPP_INCLUDED
#define Bug1200BOOST_MPL_INTEGRAL_C_HPP_INCLUDED

#define AUX_WRAPPER_VALUE_TYPE long
#include "integral_wrapper.hpp"
#define AUX_WRAPPER_PARAMS(N) typename T, T N
#define AUX_WRAPPER_NAME integral_c
#define AUX_WRAPPER_VALUE_TYPE T
#define AUX_WRAPPER_INST(value) integral_c<T, value>
#include "integral_wrapper.hpp"

#endif
