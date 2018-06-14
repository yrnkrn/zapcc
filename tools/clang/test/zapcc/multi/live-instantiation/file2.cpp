#include "file2.h"
void fn1() { Bug530callDefaultCtor<Bug530SpillPlacement>; }
void Bug530SpillPlacement::releaseMemory() { delete nodes; }
