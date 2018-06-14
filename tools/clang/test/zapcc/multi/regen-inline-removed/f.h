#pragma once
bool Bug473Contains();
struct Bug473S2LatLngRect {
  static inline void Empty();
  virtual void VirtualContainsPoint() { Bug473Contains(); }
};
void Bug473S2LatLngRect::Empty() { Bug473S2LatLngRect(); }
