#pragma once
template <class> struct Bug1936registered_base { static int converters; };
int registry_lookup1();
template <class T>
int Bug1936registered_base<T>::converters = registry_lookup1();
struct arg_rvalue_from_python {
  arg_rvalue_from_python();
};
inline arg_rvalue_from_python::arg_rvalue_from_python() {
  Bug1936registered_base<int>::converters;
}
