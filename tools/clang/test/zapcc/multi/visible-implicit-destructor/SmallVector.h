#pragma once
struct Bug2565SmallVectorImpl {
  ~Bug2565SmallVectorImpl();
};
template <typename> struct Bug2565SmallVector : Bug2565SmallVectorImpl {};
