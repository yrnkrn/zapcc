// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// RUN: %zapccxx -fsyntax-only -std=c++11 -debug-only=zapcc-files-list %s 2>&1 | FileCheck %s -allow-empty
// CHECK-NOT: bug2558.h U
#include "bug2558.h"
class abstract_read_executor
    : public enable_shared_from_this<abstract_read_executor> {
  void make_digest_requests() { shared_ptr<abstract_read_executor>(this); }
};
