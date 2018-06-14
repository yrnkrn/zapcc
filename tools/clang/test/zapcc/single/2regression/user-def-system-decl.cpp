// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 478
struct Bug478_jmp_buf_tag {};
#include "user-def-system-decl.h"
extern "C" int puts(const char *);
int main() {
  puts("user-def-system-decl");
}
