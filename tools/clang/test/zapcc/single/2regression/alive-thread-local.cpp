// RUN: %zap_compare_object
// Issue 21
#include "alive-thread-local.h"
void Issue21f2() { Issue21A a; }
