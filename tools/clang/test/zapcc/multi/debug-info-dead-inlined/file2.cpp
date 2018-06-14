#include "f.h"
extern "C" int puts(const char *s);
void DebugInfoDeadInlineFoo() {
  DebugInfoDeadInlineStruct().lookup();
}
int main() {
  puts("debug-info-dead-inlined");
}
