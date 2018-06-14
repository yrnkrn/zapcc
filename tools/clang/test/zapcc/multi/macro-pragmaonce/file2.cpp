#include "mpo_wchar.h"
extern "C" int puts(const char *s);
int main() {
  ::mpo_mbstate_t2 m;
  puts("macro-pragmaonce");
}
