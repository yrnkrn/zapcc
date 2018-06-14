#include "n2.h"
struct use_swap {
  std::StdArray<int> data;
  void swap() { data.swap(); }
};
extern "C" int puts(const char *s);
int main() {
  puts("namespace-std-std-std");
}
