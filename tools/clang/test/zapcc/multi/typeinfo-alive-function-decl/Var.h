#pragma once
template <class> class Bug1927VarHolderImpl;
template <typename T> void Bug1927Var(T val) { Bug1927VarHolderImpl<T> x(val); }
