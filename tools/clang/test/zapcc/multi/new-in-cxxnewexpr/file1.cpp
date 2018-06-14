#include "f.h"
void *Bug435Cache::operator new(unsigned long) { 
  return Bug435getSlowCase<Bug435Cache>();
}
