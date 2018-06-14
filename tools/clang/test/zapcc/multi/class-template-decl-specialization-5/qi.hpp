#pragma once
#include "my_parse.hpp"
template <typename T> void Bug1869create_parser() {}
template <> struct Bug1869parse_impl<char> {
  template <typename Iterator> static bool call(Iterator) {
    Bug1869create_parser<char>();
  }
};
