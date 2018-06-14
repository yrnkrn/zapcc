#pragma once
template <typename> struct Bug374Base {
  Bug374Base() {}
};
template <typename T> struct Bug374Derived : Bug374Base<T> {};
