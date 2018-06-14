// RUN: %zap_compare_object
// Bug 741
class Bug741allocator;
#include "unload-fe.h"
template <typename, typename> class Bug741basic_string;
void Bug741fn1(Bug741basic_string<int>);
