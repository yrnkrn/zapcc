// RUN: %zap_compare_object
// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 792
template <typename = int> struct Bug792end;
#include "template-default-arg-user.h"
