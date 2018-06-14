// RUN: %zap_compare_object
// Bug 191
#include "vector-dependent-struct.h"
extern "C" int puts(const char *);
struct VectorDependentStructHelper {
  VectorDependentStructHelper() {}
};
int main() {
  VectorDependentStructContainer c;
  puts("vector-dependent-struct");
}
