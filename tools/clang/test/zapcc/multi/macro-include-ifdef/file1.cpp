#define GUARD_ME_MACRO_IFDEF
#include "algebra.h"
extern "C" int puts(const char *);
int main() {  
  puts("macro-include-ifdef");
}
