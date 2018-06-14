// RUN: %zap_duplicate
// Bug 119
#include "duplicate-macroall-function.h"
int main() {
  DuplicateMacrotypeAll("1.0");
  DuplicateMacrotypeAll(1.0);
}
void DuplicateMacrotypeAll(const char *) {}
void DuplicateMacrotypeAll(double) {}
