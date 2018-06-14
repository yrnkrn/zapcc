// RUN: %zap_compare_object
#include "header-needs-user-destructor.h"
struct delete_implementation {
  ~delete_implementation() {}
};
int main() {
  delete_example my_example;
}
