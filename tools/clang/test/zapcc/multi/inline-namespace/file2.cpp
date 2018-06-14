#include "f.h"
void Bug461fn() {}
extern "C" int puts(const char *s);
int main() {
  puts("inline-namespace");
}
