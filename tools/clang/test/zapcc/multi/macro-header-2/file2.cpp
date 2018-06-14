#define MACRO_HEADER_2 1
#include "m.h"
extern "C" int puts(const char *);
int main() { MACRO_HEADER_2_PUTS("macro-header-2"); }
