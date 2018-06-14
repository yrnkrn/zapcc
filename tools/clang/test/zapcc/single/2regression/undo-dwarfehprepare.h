#pragma once
struct Bug1884range_error {
  ~Bug1884range_error();
};
void Bug1884foo() {
  Bug1884range_error r;
  throw 1;
}
