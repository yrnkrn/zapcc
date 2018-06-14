#pragma once
struct Bug388locale {
  Bug388locale();
  template <typename T> Bug388locale(T) {}
};
struct Bug388ios_base {
  Bug388locale l;
};
template <typename> struct Bug388num_get {
  template <typename _ValueT> char _M_extract_int() {
    (void)Bug388ios_base().l;
    return 0;
  }
  virtual void do_get() { 
    _M_extract_int<char>();
  }
};
extern template struct Bug388num_get<char>;
