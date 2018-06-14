#include "parse.hpp"
template <typename Iterator> void parse(Iterator first) {
  Bug1735parse_impl<char>::call(first, 1);
}
void foo2() { parse(""); }
