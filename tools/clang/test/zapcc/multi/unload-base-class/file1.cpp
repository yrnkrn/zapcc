#include "Interpreter.h"
template < int i > void StaticScopeIter< i >::module() {}
void module() { StaticScopeIter< 0 >().module(); }
