#pragma once
struct Bug1001ad {
  ~Bug1001ad() {}
};
unsigned Bug1001bundef();
Bug1001ad Bug1001dundef();
struct Bug1001vector {
  Bug1001vector() {}
  Bug1001vector(Bug1001vector &) : Bug1001vector(Bug1001dundef()) {}
  Bug1001vector(Bug1001ad) { ab(Bug1001bundef()); }
  Bug1001ad e;
  void ab(unsigned) {
    if (Bug1001bundef())
      Bug1001bundef();
  }
};
template <typename> struct Bug1001BlockVector { Bug1001BlockVector(); };
void Bug1001reinit(Bug1001vector);
