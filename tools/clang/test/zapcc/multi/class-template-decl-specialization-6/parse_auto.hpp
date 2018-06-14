#pragma once
#include "parse.hpp"
template <> struct Bug1735parse_impl<char> {
  template <typename Iterator> static void call(Iterator &, char const &) {}
};
