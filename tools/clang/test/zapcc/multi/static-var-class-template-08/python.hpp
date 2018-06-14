#pragma once
namespace std {
class type_info;
}
template <class T> int Bug1877type_id() { typeid(T); }
template <class> struct Bug1877registered_base { static int converters; };
template <class T> int Bug1877registered_base<T>::converters = Bug1877type_id<T>();
template <class T> inline void Bug1877register_shared_ptr_from_python_and_casts(T) {
  Bug1877registered_base<T>::converters;
}
