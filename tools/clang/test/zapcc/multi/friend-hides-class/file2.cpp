#include "file2.h"
#include "file1.h"
using namespace FriendHidesNamespace;
FriendHidesClass *F;
extern "C" int puts(const char *s);
int main() {
  puts("friend-hides-class");
}
