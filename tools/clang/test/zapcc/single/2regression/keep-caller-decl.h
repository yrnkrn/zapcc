#pragma once
void Bug549fn1();
void Bug549fn2();
struct Bug549HashingByteStreamer {
  virtual void foo() { Bug549fn2(); }
};
