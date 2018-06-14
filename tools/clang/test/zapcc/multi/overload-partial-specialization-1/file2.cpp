#include "file1.h"
template <typename T> struct OverloadPartialSpecialization1<T, T> { typedef int type; };
extern "C" int puts(const char *s);
int main() {
  OverloadPartialSpecialization1<int, int>::type i; 
  puts("overload-partial-specialization-1");
}
