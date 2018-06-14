// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1783
struct Bug1783dft_tag;
#include "user-struct-ta.h"
void foo() { Bug1783generic_hook<int>().a; }
