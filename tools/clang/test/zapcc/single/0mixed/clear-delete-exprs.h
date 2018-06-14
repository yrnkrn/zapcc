#pragma once
template <class T> struct ClearDeleteExprs {
  T *p;
  ClearDeleteExprs() {}
  ClearDeleteExprs(ClearDeleteExprs &) {}
  ~ClearDeleteExprs() {
    if (p)
      delete p;
  }
};
