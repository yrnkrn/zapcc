#include "file1.h"
template <typename> class Bug950ArrayRef {};
void Bug950foo2() {
  Bug950dd(Bug950ArrayRef<int>());
}
