// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1432
#include "declcontext-peers.h"
namespace Bug1432JS {
typedef char AutoIdVector;
}
namespace Bug1432js {
using Bug1432JS::AutoIdVector;
typedef char AutoIdVector;
}
