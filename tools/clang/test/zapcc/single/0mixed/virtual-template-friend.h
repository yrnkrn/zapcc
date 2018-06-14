#pragma once
struct IDcollection {
  IDcollection(int ID) {}
};
template <typename T> struct TroubleID {
  virtual void justFunc() { IDcollection I(T::ID); }
  friend struct JustID;
};
