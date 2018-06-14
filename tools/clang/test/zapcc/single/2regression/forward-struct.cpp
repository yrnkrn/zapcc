// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "forward-struct.h"
struct ForwardS {
  int Kind;
};
int main() {
  ForwardS s;
  s.Kind = 1;
}
