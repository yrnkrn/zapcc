#pragma once
class LambdaInHeaderStructBase {
  int value;
  int *value2;
};
struct LambdaInHeaderStructDerived : LambdaInHeaderStructBase {
  template <typename T>
  LambdaInHeaderStructDerived(T)
      : LambdaInHeaderStructBase() {}
  int *member;
};
template <typename T> void LambdaInHeaderFunction3(T) {
  LambdaInHeaderStructDerived F([] {});
}
