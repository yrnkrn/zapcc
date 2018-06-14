#include "virtual-destructor.h"
void fn1() { VirtualDestructorCreate(); }
extern "C" int puts(const char *s);
int main() {
  puts("virtual-destructor");
}
