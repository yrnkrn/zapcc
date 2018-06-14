#pragma once
template <typename T> struct Bug501PerThread {
  static T *getSlowCase() { new T; }
};
struct Bug501Cache {
  void operator delete(void *, unsigned long);
  static void *tryAllocate();
};
inline void *Bug501Cache::tryAllocate() { Bug501PerThread<Bug501Cache>(); }
