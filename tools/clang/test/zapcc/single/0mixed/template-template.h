#pragma once
inline void pointer(void *) {}
template <typename G> struct template_template_S {
  template <typename T> template_template_S(T) { pointer(p); }
  void *p;
};
