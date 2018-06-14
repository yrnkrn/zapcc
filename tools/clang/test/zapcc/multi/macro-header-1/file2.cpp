#define MACRO_HEADER_1
#include "m.h"
extern "C" int puts(const char *);
int main() { MACRO_HEADER_1_PUTS("macro-header-1"); }
