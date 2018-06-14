#include "file1.h"
void Bug556swap2();
void Bug556TripleBandLinearOp::swap1() {
  Bug556swap2();
}
void Bug556TripleBandLinearOp::mult() {
  Bug556Disposable<Bug556TripleBandLinearOp> D(*this);
}
