// RUN: %zap_compare_object
#include "template-static-user.h"
void TemplateStaticUserFunc() { 
  TemplateStaticUserClass<char>().foo();
}
template <typename Char> void TemplateStaticUserClass<Char>::foo() {}
int main() { }
