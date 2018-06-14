#pragma once
template <typename> struct IE2_Struct {
  void TheFunction() {}
};
extern template struct IE2_Struct<int>;
