#include "f.h"
template <int dim> struct A {
  void bar() {
    Bug329NS::Bug329TemplateStruct<dim> a;
    a.template foo<dim>();
  }
};
template struct A<3>;
extern "C" int puts(const char *s);
int main() {
  puts("inverse-dependency");
}
