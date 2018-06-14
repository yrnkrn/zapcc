#pragma once
template <typename T> struct explicit_instantiation_S {
  void Sfunc() {}
};
struct explicit_instantiation_P {
  explicit_instantiation_S<explicit_instantiation_P> s;
};
