// RUN: %zap_duplicate
#include "duplicate-virtual-thunk.h"
bool VTClassGlobal;
bool VTClass3::foo() { return VTClassGlobal; }
