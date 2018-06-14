#pragma once
#include "file2.h"
template <class> struct Bug639DominatorTreeBase;
static void foo1() { Bug639DominatorTreeBase<Bug639MachineBasicBlock> a; };
