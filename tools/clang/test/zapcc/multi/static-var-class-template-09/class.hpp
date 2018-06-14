#pragma once
template <class> struct Bug1812registered_base { static int converters; };
template <class T> int Bug1812registered_base<T>::converters;
struct Bug1812arg_rvalue_from_python { Bug1812arg_rvalue_from_python(); };
Bug1812arg_rvalue_from_python::Bug1812arg_rvalue_from_python() {
  Bug1812registered_base<int>::converters;
}
