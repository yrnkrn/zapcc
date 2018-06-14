#pragma once
struct UserStruct;
template <typename T> struct SystemStruct {
  virtual int justFunc() { return T::anchor; }
};
