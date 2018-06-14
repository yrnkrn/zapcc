// RUN: %zap_compare_object
// RUN: %zap_compare_object
#include "inline-anonymous-namespace.h"
namespace {
int Bug484;
}
extern "C" int puts(const char *);
int main() {
  puts("inline-anonymous-namespace");
}
