#pragma once
struct atomic_base {
  void use0() {
    int pointer;
    __atomic_compare_exchange(&pointer, 0, 0, 0, 0, 0);
  }
};
