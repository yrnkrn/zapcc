#pragma once
template <class T> struct Bug1869parse_impl {
  template <typename Iterator> static bool call(Iterator) {}
};
