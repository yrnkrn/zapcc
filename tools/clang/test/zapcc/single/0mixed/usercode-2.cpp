// RUN: %zap_compare_object
#include "usercode-2.h"
template <typename T> T &operator*(codeater<usercode2<T>> &c) { return c.t.val; }
int main() {
  codeater<> c;
}
