#include "math.h"
void Bug1165a(Bug1165function<void()>);
template <class a> void Bug1165b(a) {}
void Bug1165b() {
  Bug1165a([] {});
}
