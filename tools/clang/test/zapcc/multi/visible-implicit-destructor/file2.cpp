#include "SmallVector.h"
void foo2() {
  Bug2565SmallVector<unsigned>().~Bug2565SmallVector();
}
