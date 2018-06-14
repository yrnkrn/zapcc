#pragma once
struct Bug1017MatchExpression {
  enum MatchType {};
  virtual void a();
};
struct Bug1017ElementIterator {
  ~Bug1017ElementIterator();
};
void foo2h() { delete (Bug1017ElementIterator *)nullptr; }
