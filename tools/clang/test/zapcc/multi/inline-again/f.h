#pragma once
void Bug413fn2();
struct Bug413InputArray {
  static void InlineFunc();
};
inline void Bug413InputArray::InlineFunc() { Bug413fn2(); }
