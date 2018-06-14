// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 232
#include "visit-linkagespec.h"
namespace VisitLinkageSpecNS {
extern "C" void bar() {}
}
extern "C" int puts(const char *);
int main() { 
  VisitLinkageSpecNS::bar();
  puts("visit-linkagespec");
}
