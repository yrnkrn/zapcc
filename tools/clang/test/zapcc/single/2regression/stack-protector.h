#pragma once
struct Bug1177CLHashMap {
  void goo(char &);
  virtual void put(char k) { goo(k); }
};
