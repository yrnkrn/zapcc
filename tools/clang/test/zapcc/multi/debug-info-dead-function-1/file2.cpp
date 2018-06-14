#include "f.h"
struct A : DIDF_ByteQueue {
  A(DIDF_BufferedTransformation) {}
};
void fn1() {
  DIDF_BufferedTransformation xy;
  A seq(xy);
}
DIDF_BufferedTransformation::~DIDF_BufferedTransformation() {}
int DIDF_BufferedTransformation::MaxRetrievable() {
  return 0;
}
