#include "f.h"
namespace {
struct DebugInfoDeadInlineS {};
}
void DebugInfoDeadInlineStruct::lookup() { fetch(); }
void DebugInfoDeadInlineStruct::fetch() {
  DebugInfoDeadInline_find_if(DebugInfoDeadInlineS());
}
