#pragma once
struct Bug1058ios_base {
  ~Bug1058ios_base();
};
template <class> struct Bug1058basic_ios : Bug1058ios_base {
  ~Bug1058basic_ios() {}
};
