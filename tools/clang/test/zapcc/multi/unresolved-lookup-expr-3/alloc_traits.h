template <typename _Alloc> struct Bug1270__alloc_traits {
  auto construct() -> decltype(Bug1270forward<_Alloc>);
};
