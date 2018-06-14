#pragma once
template <class> struct Bug1981lanczos_initializer { static int initializer; };
template <class T> int Bug1981lanczos_initializer<T>::initializer;
template <class T> void Bug1981foo(T z) { Bug1981lanczos_initializer<T>::initializer; }
