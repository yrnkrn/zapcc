// RUN: %zap_compare_object
#include "system-system-user-function.h"
void g(su_call_class) {
}
int main() { 
  su_call_f<su_call_class>();
}
