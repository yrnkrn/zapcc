#pragma once
template <typename _Tp> struct D {
  D() { new _Tp; }
};
class exception_impl;
class exception {
  exception();
  D<exception_impl> my;
};
template <typename _Args> void make_shared(_Args);
template <typename T> void copy_exception(T p1) { make_shared<exception>(p1); }
