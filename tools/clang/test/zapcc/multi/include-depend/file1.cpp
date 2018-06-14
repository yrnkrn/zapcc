template <typename, typename> class Bug1262vector;
#include "vector.h"
void foo1() { Bug1262vector<int>; }
