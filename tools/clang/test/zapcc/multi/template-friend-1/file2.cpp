#include "f.h"
extern "C" int puts(const char *s);
int main() {
  TemplateAfterFriend1<1> TAF;
  puts("template-after-friend-1");
}
