#pragma once

template <typename T> struct usercode1 {
  T val;
  usercode1() { val = 1234; }
  ~usercode1() { val = 0; }
  T &operator*(usercode1<T> &u);
};
