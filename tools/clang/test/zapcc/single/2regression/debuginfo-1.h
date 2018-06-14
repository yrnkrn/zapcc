#pragma once
template <typename _Tp> struct DI_make_unsigned { typedef _Tp type; };
template <typename _Tp> struct DI_allocator_traits {
  typedef _Tp T;
  static int _S_difference_type_helper();
  typedef decltype(_S_difference_type_helper()) difference_type;
  static typename DI_make_unsigned<difference_type>::type
  _S_unsignedype_helper();
};
template <typename _Tp> struct DI_priority_queue {
  typename DI_allocator_traits<_Tp>::T d;
};
