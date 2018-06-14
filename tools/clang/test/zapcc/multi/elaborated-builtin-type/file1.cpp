#include "file1.h"
#include "file2.h"
void Bug301d() {
  Bug301forward([] {});
}
