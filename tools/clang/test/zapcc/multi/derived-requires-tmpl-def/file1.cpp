#include "file1.h"
namespace Bug702dealii {
class S {};
void Bug702foo1() { Accessor<S> a; }
}
#include "file2.h"
