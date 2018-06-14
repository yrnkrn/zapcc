// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 376
#include "dynamiccastexpr.h"
class LogStorageProcess : public Process<LogStorageProcess> {};
extern "C" int puts(const char *s);
int main() {
  LogStorageProcess process;
  dispatch(&process);
  puts("dynamiccastexpr");
}
