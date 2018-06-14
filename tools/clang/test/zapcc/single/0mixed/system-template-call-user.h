#pragma once
class st_call_class {};
template <typename T> struct st_call_g {
  st_call_g() { g(T()); }
};
