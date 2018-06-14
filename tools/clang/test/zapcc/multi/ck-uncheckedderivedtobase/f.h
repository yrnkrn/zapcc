#pragma once
struct Bug1400st2 {
  struct st {
    void ref();
  } ref;
};
template <class T> void Bug1400cl2(T *adata) { adata->ref.ref(); }
struct Bug1400Base;
