#pragma once
template <typename> class Bug535basic_string {};
void Bug535hash_value(const Bug535basic_string<char> &);
template <typename T> void Bug535hash_value(Bug535basic_string<T> &p1) { Bug535hash_value(p1); }
