#pragma once
template <typename> struct Bug2572standard_migrator {
  Bug2572standard_migrator() {}
  void foo() {}
  static Bug2572standard_migrator object;
};
template <typename T> Bug2572standard_migrator<T> Bug2572standard_migrator<T>::object;
struct Bug2572allocation_strategy {
  virtual void alloc();
  template <typename T> void construct() { Bug2572standard_migrator<T>::object.foo(); }
};
