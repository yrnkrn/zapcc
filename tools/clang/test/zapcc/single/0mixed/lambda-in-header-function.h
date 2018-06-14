#pragma once
struct LambdaInHeaderStruct {
  template <typename T> LambdaInHeaderStruct(T) {}
};
template <typename T> void LambdaInHeaderFunction() {
  auto a = ([] {});
  LambdaInHeaderStruct s(&a);
}
