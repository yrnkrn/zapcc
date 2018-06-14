#pragma once
extern int UAF2[];
struct UAF2Struct {
  static int getUAF() { return UAF2[0]; }
};
