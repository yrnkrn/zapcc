#pragma once
class call_class {};
template <typename T> struct call_g {
  void f() { g(T()); }
};
void call_f() {
  call_g<call_class>().f();
};
