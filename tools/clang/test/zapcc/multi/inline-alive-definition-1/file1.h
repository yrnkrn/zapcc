#include "file2.h"
void Bug1001reinit(Bug1001vector);
template <typename a> Bug1001BlockVector<a>::Bug1001BlockVector() {
  Bug1001vector b;
  Bug1001reinit(b);
}
