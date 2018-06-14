// RUN: %zap_compare_object
#include "usercode-3.h"
struct implementation3 {
  void print() {
  }
};
int main() {
  usercode3<implementation3> u;
}
