#pragma once
template <typename> struct Bug2547basic_string {
  ~Bug2547basic_string() {}
};
extern template class Bug2547basic_string<char>;
