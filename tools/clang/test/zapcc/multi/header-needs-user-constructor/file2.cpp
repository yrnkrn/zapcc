#include "header-needs-user-constructor.h"
struct new_explicit_implementation {
  new_explicit_implementation() {}
};
int main() {
  new_explicit_example my_example;
}
