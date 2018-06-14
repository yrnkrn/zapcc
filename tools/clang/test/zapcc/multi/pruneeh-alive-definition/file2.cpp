#include "file2.h"
void foo2() {
  Bug1016BlockVector e;
  Bug1016ConstraintMatrix().condense(e);
}
