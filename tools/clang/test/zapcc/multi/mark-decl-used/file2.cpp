#include "mark-decl-used.h"
void MarkDeclUsedFunction(MarkDeclUsed2 &m) { m.isNull(); }
extern "C" int puts(const char *);
int main() {
  puts("mark-decl-used");
}
