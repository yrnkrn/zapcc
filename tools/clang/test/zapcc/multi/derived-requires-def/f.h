#pragma once
template <long> struct Bug331_Index_tuple {};
template <typename> struct Bug331_Bind {
  template <long... _Indexes> void m_fn1(Bug331_Index_tuple<_Indexes...>);
};
template <typename T> struct Bug331Promise {
  Bug331_Bind<T()> associate() {}
};
struct Bug331A;
struct Bug331A {
  Bug331Promise<int> a;
};
struct Bug331Process : Bug331A {};
