// RUN: %zap_compare_object
namespace NSmixed {
struct NSmixedS;
}
#include "first-namespace.h"
int main() { 
  NSmixed::NSmixedS s;
}
