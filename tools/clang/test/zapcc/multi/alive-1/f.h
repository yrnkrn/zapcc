#pragma once
struct Bug1097exception {
  Bug1097exception();
  virtual ~Bug1097exception();
};
struct Bug1097SystemException : Bug1097exception {
  ~Bug1097SystemException();
};
void foo1() { throw Bug1097SystemException(); }
