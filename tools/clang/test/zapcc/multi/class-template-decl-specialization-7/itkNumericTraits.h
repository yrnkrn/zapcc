#pragma once
template <typename T> struct Bug2343NumericTraits {
  static int GetLength() {}
};
template <typename> struct Bug2343vector {};
