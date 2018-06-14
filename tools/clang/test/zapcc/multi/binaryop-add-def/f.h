#pragma once
struct Bug663BuildsystemDirectoryStateType;
template <typename T> struct Bug663vector {
  T *foo() {
    T *p;
    return p + 1;
  }
};
