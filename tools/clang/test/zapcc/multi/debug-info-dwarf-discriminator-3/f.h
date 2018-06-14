#pragma once
struct Bug400A {
  virtual void maxSwapTenor() {}
  virtual void volatilityImpl() {}
};
struct Bug400F {
  virtual void performCalculations() {}
};
void Bug400__assert_fail();
struct Bug400C {
  Bug400A* foo() {
    px ? void() : Bug400__assert_fail();
    return px;
  }
  Bug400A *px;
};
struct Bug400G : Bug400F, Bug400A {
  void maxSwapTenor() override;
  void volatilityImpl() override{ atmVol_.foo()->maxSwapTenor(); }
  Bug400C atmVol_;
};
inline void Bug400G::maxSwapTenor() {}
