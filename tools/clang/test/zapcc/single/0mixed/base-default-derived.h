#pragma once
template <typename> class default_Base {};
struct default_Derived : default_Base<int> {
  default_Derived();
};
