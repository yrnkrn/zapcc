// RUN: %zap_compare_object
#include "dc-namespace.h"
namespace dcname {
}
void dcname::func() {
}
namespace nested1 {
namespace nested2 {
}
}
void nested1::nested2::deep() {
}
int main() {
  dcname::func();
}
