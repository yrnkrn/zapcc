// RUN: %zap_compare_object CXXFLAGS="-O"
// Bug 1013
void f();
void call(void (*aa)()) { aa(); }
void foo1() { call(f); }
