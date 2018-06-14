#include "f.h"
extern "C" int puts(const char *s);
int main() {
  Bug451ConstantExpr b;
  puts("destructor-operatordelete");
}
