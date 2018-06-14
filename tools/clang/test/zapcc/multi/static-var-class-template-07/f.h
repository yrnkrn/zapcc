#pragma once
struct Bug1857_exception_ptr {
  ~Bug1857_exception_ptr();
};
template <class> struct Bug1857_exception_ptr_static_exception_object {
  static Bug1857_exception_ptr e;
};
template <class Exception>
Bug1857_exception_ptr Bug1857_exception_ptr_static_exception_object<Exception>::e;
inline void Bug1857_current_exception_impl() {
  Bug1857_exception_ptr_static_exception_object<int>::e;
}
