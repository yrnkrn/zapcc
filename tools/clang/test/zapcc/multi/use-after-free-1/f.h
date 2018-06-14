#pragma once
struct UAFStructBase {
  int S = 0;
};
struct UAFStructDerived : UAFStructBase {};
struct UAF {
  static UAFStructDerived StaticUAF;
  static int getS() { return StaticUAF.S; }
};
