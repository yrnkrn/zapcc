#include "file1.h"
#include "file2.h"
class Bug981Token {};
void Bug981foo1() { Bug981unique_ptr<Bug981Token>().foo(); }
