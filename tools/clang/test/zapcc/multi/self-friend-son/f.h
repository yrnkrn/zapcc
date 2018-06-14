#pragma once
template <typename T> class MultiIfriend;
struct MultiIself {
  friend class MultiIfriend<MultiIself>;
};
