#include "math.h"
void Bug1165a(Bug1165function<void()>);
void Bug1165b() {
  Bug1165a([] {});
}
