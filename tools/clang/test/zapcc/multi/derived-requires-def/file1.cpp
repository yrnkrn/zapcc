#include "f.h"
struct Bug331B {};
void Bug331C() {
  Bug331_Bind<Bug331B>();
  Bug331Promise<int>().associate();
}
