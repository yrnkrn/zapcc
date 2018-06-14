template <class> struct Bug1812_registered_base { static int converters; };
template <class> struct Bug1812_registered : Bug1812_registered_base<int> {};
template <class T> int Bug1812_registered_base<T>::converters;
template <class Caller> struct Bug1812_caller_py_function_impl {
  virtual void foo() { Bug1812_registered<Caller>::converters; }
};
