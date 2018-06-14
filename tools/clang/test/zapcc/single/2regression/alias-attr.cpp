// RUN: %zap_compare_object
// Bug 1020
extern "C" void foo2();
void foo1() __attribute__((alias("foo2")));
static void foos() { foo1(); }
void foo2() { foos(); }
