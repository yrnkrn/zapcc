// RUN: %zap_duplicate
// Bug 116
#include "duplicate-macrotype-function.h"
int main() {
  DuplicateMacrotypeFunction("1.0");
  DuplicateMacrotypeFunction(1.0);
}
VOID DuplicateMacrotypeFunction(const char *) {}
VOID DuplicateMacrotypeFunction(double) {}
