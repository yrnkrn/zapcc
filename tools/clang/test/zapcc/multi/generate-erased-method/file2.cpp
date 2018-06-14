#include "f.h"
struct A : DIDF2_ByteQueue {
  A(DIDF2_BufferedTransformation) {}
};
void fn1() {
  DIDF2_BufferedTransformation xy;
  A seq(xy);
}
DIDF2_BufferedTransformation::~DIDF2_BufferedTransformation() {}
int DIDF2_BufferedTransformation::MaxRetrievable() {
  return 0;
}

extern "C" int puts(const char *s);
int main() {
  puts("debug-info-dead-function-2");
}
