#pragma once
struct Bug1467cl1 {
  virtual ~Bug1467cl1();
  virtual void anchor();
};
struct Bug1467cl2 : Bug1467cl1 {
  void anchor();
};
