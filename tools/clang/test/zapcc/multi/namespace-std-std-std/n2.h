#pragma GCC system_header
#pragma once
template <typename T>
void GlobalSwap(T, T);

namespace std {
template <typename T>
struct StdArray {
  void swap() noexcept(noexcept(GlobalSwap(0, T()))) {}
};
}
