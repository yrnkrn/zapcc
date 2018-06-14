// RUN: %zap_compare_object CXXFLAGS="-isystem %S"
// RUN: %zap_compare_object CXXFLAGS="-isystem %S"
#include <system-header-var.h>
extern "C" int puts(const char *);
int main() {
  SystemHeaderVar = 0;
  puts("system-header-var");
}
