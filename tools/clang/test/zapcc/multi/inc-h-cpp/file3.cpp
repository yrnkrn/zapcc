#include "inc-h-cpp.h"
static void foo3() { (void)Inc_H_Cpp; }
extern "C" int puts(const char *);
int main() {
  puts("inc-h-cpp");
}
