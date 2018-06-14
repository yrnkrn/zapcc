// RUN: %zap_compare_object
#include "usercode-1.h"
template <typename T> T operator*(usercode1<T> &u) { return u.val; }
int main() {
  usercode1<int> u;
}
