#pragma once
template <typename _Tp> struct Bug1440unique_ptr {
  int x;
  Bug1440unique_ptr(_Tp *p) { delete p; }
};
struct Bug1440MatchExpression {
  virtual ~Bug1440MatchExpression();
  virtual void debugString();
};
struct Bug1440NotMatchExpression : Bug1440MatchExpression {
  void debugString();
};
