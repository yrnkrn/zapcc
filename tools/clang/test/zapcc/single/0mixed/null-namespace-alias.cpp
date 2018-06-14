// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 273
#include "null-namespace-alias.h"
namespace internal {
template <typename T> void foo(T) {}
}
template class NullNamespaceAlias<0>;
template <int n> void NullNamespaceAlias<n>::bar() { internal::foo(s); }
extern "C" int puts(const char *s);
int main() {
  puts("null-namespace-alias");
}
