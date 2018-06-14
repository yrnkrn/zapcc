#pragma once
struct Bug435Cache {
  void *operator new(unsigned long);
  static void tryAllocate();
};
template <typename T> T *Bug435getSlowCase() { return new T; }
inline void Bug435Cache::tryAllocate() { Bug435getSlowCase<Bug435Cache>(); }
