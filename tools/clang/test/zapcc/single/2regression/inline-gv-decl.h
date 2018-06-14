#pragma once
struct Bug347Base {
  virtual ~Bug347Base();
  virtual void print() {}
};
class Bug347Derived : Bug347Base {
  void print();
};
