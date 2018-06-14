#pragma once
template <typename T> class SingleIfriend;
struct SingleIself {
  friend class SingleIfriend<SingleIself>;
};
