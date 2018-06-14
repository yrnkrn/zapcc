#pragma once
template <int> struct Bug1653Function;
template <int dim> struct Bug1653CellAccessor {
  void subface_case();
  friend Bug1653Function<dim>;
};
template <> void Bug1653CellAccessor<1>::subface_case() {}
