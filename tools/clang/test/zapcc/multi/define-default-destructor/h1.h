#pragma once
template <typename T> struct Bug1819unique_ptr {
  ~Bug1819unique_ptr() { delete T(); }
};
struct Bug1819cl;
