#pragma once

template <typename T> struct usercode2 {
  T val;
  usercode2() { val = 12345; }
  ~usercode2() { val = 0; }
};

template <typename T = usercode2<int>> struct codeater {
  T t;
  codeater() {}
  T &operator*(codeater<T> &c);
};
