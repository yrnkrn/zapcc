// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "struct-reference.h"
extern "C" int puts(const char *s);
int main() { 
  calc(nullptr);
  puts("struct-reference");
}
struct M {
  unsigned x;
};
void calc(M *) {}
