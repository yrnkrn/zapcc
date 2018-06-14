#include "f0.h"
#include "f1.h"
extern "C" int puts(const char *s);
int main() {
  RedefineEnumDeclDef1 r1;
  RedefineEnumDeclDef2 r2;
  puts("redefine-enum-decl-def");
}
