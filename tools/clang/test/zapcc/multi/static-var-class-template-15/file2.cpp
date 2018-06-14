#include "lithium.h"
template <int kNumCachedOperands>
int Bug2057LSubKindOperand<kNumCachedOperands>::cache;
void LOperandSetUpCaches() { Bug2057LSubKindOperand<16>::cache; }
