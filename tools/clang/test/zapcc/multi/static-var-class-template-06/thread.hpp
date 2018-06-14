#pragma once
struct Bug1857shared_ptr {
  ~Bug1857shared_ptr();
};
struct Bug1857exception_ptr {
  Bug1857shared_ptr ptr_;
};
template <class> struct Bug1857exception_ptr_static_exception_object {
  static Bug1857exception_ptr e;
};
template <class Exception>
Bug1857exception_ptr Bug1857exception_ptr_static_exception_object<Exception>::e;
void Bug1857current_exception_impl() { Bug1857exception_ptr_static_exception_object<int>::e; }
