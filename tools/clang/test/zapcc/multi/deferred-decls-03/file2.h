#pragma once
#ifndef Bug559STL_PAIR_H
#include "file1.h"
#endif
template <typename T> struct Bug559map {
  Bug559map(Bug559pair<T>);
};
