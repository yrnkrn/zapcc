// RUN: %zap_compare_object
#include "template-template.h"
struct Structure {};
int main() {
  template_template_S<Structure> s(0);
}
