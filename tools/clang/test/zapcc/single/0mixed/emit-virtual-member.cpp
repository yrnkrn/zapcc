// RUN: %zap_compare_object CXXFLAGS="-O"
#include "emit-virtual-member.h"
void foo1() {
  evm_ByteQueue member;
  member.AnyRetrievable();
}
