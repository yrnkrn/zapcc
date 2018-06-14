// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1435
struct Bug1435MCSubtargetInfo {
  struct basic_string {
    ~basic_string();
  } CPU;
};
#include "cxxdeleteexpr-destructor.h"
void foo() { (void)Bug1435unique_ptr<Bug1435MCSubtargetInfo>; }
