#pragma once
template <int> struct Bug304Base { 
  struct Implementation;
};
struct Bug304Derived : Bug304Base<0> {};
