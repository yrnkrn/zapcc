#include "type_traits.h"
#include "unique_ptr.h"
#include "PointerLikeTypeTraits.h"
struct Bug906a;
void Bug906foo1() { Bug906std2::unique_ptr<Bug906a> b; }
