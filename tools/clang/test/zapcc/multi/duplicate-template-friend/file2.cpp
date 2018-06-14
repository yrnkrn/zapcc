#include "bu.h"
DuplicateTemplateFriend<2> b2;
extern "C" int puts(const char *s);
int main() {
  puts("duplicate-template-friend");
}
