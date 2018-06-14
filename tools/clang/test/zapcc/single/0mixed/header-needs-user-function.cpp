// RUN: %zap_compare_object
#include "header-needs-user-function.h"
void g(call_class) {
}
int main() { call_f(); }
