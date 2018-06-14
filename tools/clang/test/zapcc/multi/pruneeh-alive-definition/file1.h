#include "file2.h"
void Bug1016ConstraintMatrix::condense(Bug1016BlockVector &c) { condense(c, c); }
