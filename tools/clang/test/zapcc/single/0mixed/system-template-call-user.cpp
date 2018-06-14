// RUN: %zap_compare_object
#include "system-template-call-user.h"
void g(st_call_class) {}
int main() { 
  st_call_g<st_call_class>();
}
