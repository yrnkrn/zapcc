#pragma once
struct CxxMethodIOBuf {
  CxxMethodIOBuf();
};

struct CxxMethodWrapper {
  template <typename T>
  void make_unique() { new CxxMethodIOBuf; }

  void clone() {
    make_unique<CxxMethodIOBuf>();
  }
};
