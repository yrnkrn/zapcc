// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1189
#include "tls-wrapper.h"
thread_local int Bug1189logging_failures;
