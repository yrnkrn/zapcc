#pragma once
class su_call_class {};
template <typename T> void su_call_g() {
  g(T());
};
template <typename T> void su_call_f() {
  su_call_g<T>();
};
