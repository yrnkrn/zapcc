#include "file2.h"
void Bug563fn1() {
  Bug563JSScript a;
  a.isDebuggee();
}
namespace {}
#include "file1.h"
