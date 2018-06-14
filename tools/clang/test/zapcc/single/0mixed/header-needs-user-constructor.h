#pragma once
struct new_explicit_implementation;
template <typename T> struct new_empty_ptr {
  T *p;
  new_empty_ptr() : p(new T) {}
};
class new_explicit_example {
  new_empty_ptr<new_explicit_implementation> imp;
};
