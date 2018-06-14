// RUN: %zap_compare_object
// Bug 291
#include "template-in-anonymous-ns.h"
class C {};
int main() { 
  TemplateInAnonymousNSClass1<TemplateInAnonymousNSClass2<C>> c;
}
