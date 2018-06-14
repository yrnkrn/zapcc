#pragma once
struct Bug1007_Sp_counted_base {
  virtual ~Bug1007_Sp_counted_base() {}
};
template <typename> struct Bug1007_Sp_counted_ptr : Bug1007_Sp_counted_base {
  Bug1007_Sp_counted_ptr() {}
};
