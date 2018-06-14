#include "variant.hpp"
struct data_type {};
void foo1() { Bug1172integral_constant<data_type>::value; }
