#pragma once

struct BaseClassThunk {
  int i;
  BaseClassThunk() {}
  virtual ~BaseClassThunk() {}
};
struct BaseClassThunkPublic : virtual BaseClassThunk {
  ~BaseClassThunkPublic();
};
struct DerivedThunk : BaseClassThunkPublic {
  ~DerivedThunk();
};
