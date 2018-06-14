#include "file1.h"
#include "file2.h"
extern "C" int puts(const char *s);
int main() {
  Bug380ExclusiveFilter e;
  puts("cxxcatchstmt");
}
