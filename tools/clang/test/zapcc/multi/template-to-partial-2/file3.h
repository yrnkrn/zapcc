#pragma once
template <bool b> struct Bug1110integral_constant {
  static constexpr bool value = b;
};
template <bool b> constexpr bool Bug1110integral_constant<b>::value;
template bool Bug1110integral_constant<false>::value;
