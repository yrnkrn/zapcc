#pragma once
struct Bug417X {};
struct Bug417Y {};
template <class T> void Bug417user_domain_error(T);
template <class T> void Bug417raise_domain_error(Bug417X) {}
template <class T> void Bug417raise_domain_error(Bug417Y) { Bug417user_domain_error(T()); }
inline void Bug417foo() { Bug417raise_domain_error<int>(Bug417X()); }
