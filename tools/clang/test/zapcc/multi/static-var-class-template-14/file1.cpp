#include "gamma.hpp"
template < class T > T binomial_coefficient();
template <> int binomial_coefficient() { Bug1981foo(0); }
