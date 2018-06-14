#include "header.h"
#ifndef HEADER2
#error header2/header.h was not included!
#endif
extern "C" int puts(const char *s);
int main() { 
  puts("same-name-headers");
}
