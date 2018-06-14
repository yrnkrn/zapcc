// RUN: %zap_compare_object
// Bug 738
#include "anon-usingdirectivedecl.h"
class Bug738Scanner {
  Bug738unique_ptr<unsigned> u;
};
Bug738Stream::Bug738Stream() {}
