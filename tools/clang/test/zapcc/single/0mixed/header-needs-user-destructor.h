#pragma once
template <typename T>
struct dumb_ptr {
  T *p;
  dumb_ptr() : p(nullptr) {}
  ~dumb_ptr() {
    if (p)
      delete p;
  }
};
struct delete_implementation;
struct delete_example {
  dumb_ptr<delete_implementation> imp;
};
