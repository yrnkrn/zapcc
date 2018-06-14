// RUN: %zap_compare_object
#include "implicit-using-directive-decl.h"
namespace {
struct UsingDirectiveDeclS {};
}
int main() {
  UsingDirectiveDeclFunc(UsingDirectiveDeclS());
}
