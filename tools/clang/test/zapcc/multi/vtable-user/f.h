#pragma once
struct Bug422bad_cast {};
struct Bug422length_error {};
struct Bug422exception {
  virtual ~Bug422exception() = 0;
};
template <class T> struct Bug422error_info_injector : T, Bug422exception {};
