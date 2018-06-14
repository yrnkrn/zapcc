#include "file2.h"
struct Bug637_GlobalObject : Bug637_User {};
#include "file1.h"
void fn1() {
  Bug637_GlobalVariable a;
  a.OpFrom(&a);
}
