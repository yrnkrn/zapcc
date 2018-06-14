void use1() {
  int pointer;
  __atomic_compare_exchange(&pointer, 0, 0, 0, 0, 0);
}
#include "file1.h"
void use2() {
  int pointer;
  __atomic_compare_exchange(&pointer, 0, 0, 0, 0, 0);
}
extern "C" int puts(const char *s);
int main() {
  puts("atomic-reinclude");
}
