#pragma once
template <typename T> void OverloadExpr_call_g() {
  OverloadExpr_g(T());
};
template <typename T> struct OverloadExpr2_call_g {
  OverloadExpr2_call_g() { OverloadExpr2_g(T()); }
};
