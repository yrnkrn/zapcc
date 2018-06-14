#pragma once
template <class> struct Bug1009basic_parser {
  Bug1009basic_parser() {}
};
extern template struct Bug1009basic_parser<bool>;
