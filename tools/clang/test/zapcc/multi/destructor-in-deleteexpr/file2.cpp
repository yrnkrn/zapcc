#include "f.h"
DestructorInDeletEexprS::~DestructorInDeletEexprS() {}
extern "C" int puts(const char *s);
int main() {
  DestructorInDeletEexprPtr<DestructorInDeletEexprS> b;
  puts("destructor-in-deleteexpr");
}
