#include "f.h"
void Bug814pthread_mutex_scoped_lock1();
void Bug814foo3() {
  Bug814pthread_mutex_scoped_lock1();
}
