// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1557
#include "unload-friend-function-def.h"
class Bug1557QVariant {};
void foo() {
  Bug1557QList<Bug1557QVariant>().at;
  Bug1557qt_getQtMetaObject();
}
