#pragma once
#include "file3.h"
class Bug1110a {
  template <typename> friend struct Bug1110DenseMapInfo;
  Bug1110a(Bug1110integral_constant<false>);
};
