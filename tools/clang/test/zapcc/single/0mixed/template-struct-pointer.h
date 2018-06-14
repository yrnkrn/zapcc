#pragma once
template <typename> class GW;
struct SD {
  void foo();
  GW<SD> *bar;
};
