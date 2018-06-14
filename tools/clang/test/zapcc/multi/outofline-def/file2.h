#pragma once
struct Inst {
  void admitsStack(unsigned);
};
inline void admitsStack() { Inst().admitsStack(0); }
