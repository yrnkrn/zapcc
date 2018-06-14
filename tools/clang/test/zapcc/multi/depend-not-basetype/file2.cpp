#include "unique_ptr.h"
#include "type_traits.h"
#include "PointerLikeTypeTraits.h"
void Bug906foo2() { (void)Bug906b<sizeof(int)>::c; }
