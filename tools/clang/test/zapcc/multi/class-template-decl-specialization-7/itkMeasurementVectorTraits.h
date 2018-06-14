#pragma once
template <typename T> struct Bug2343NumericTraits<Bug2343vector<T> > {
  static int GetLength() {}
};
