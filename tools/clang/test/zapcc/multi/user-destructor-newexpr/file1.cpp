#include "f.h"
void Bug501Cache::operator delete(void *, unsigned long) {
  Bug501PerThread<Bug501Cache>::getSlowCase();
}
