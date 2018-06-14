#pragma once
struct IFT2_Device {
  int vendorID();
  int isIntel() { return vendorID(); }
  struct Impl;
  Impl *p;
};
