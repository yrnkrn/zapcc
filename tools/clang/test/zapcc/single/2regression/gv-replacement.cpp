// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1007
#include "gv-replacement.h"
struct a {};
void f() { new Bug1007_Sp_counted_ptr<a *>(); }
int main() {
}
