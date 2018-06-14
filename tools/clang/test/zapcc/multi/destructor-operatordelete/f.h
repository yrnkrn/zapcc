#pragma once
struct Bug451Value {
  virtual ~Bug451Value() {}
  void operator delete(void *);
};
struct Bug451ConstantExpr : Bug451Value {
  Bug451ConstantExpr() : Bug451Value() {}
};
