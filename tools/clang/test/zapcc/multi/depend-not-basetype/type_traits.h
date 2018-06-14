#pragma once
namespace Bug906std2 {
template <typename a, a b> struct integral_constant {
  static constexpr a c = 0;
};
}
