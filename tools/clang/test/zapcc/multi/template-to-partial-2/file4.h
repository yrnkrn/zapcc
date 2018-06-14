#pragma once
#include "file3.h"
template <typename> struct Bug1110DenseMapInfo;
template <typename a> struct Bug1110DenseMapInfo<a *> {
  static a getTombstoneKey() { a b; }
};
template <typename> struct Bug1110DenseMapInfo;
