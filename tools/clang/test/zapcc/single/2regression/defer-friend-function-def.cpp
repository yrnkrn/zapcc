// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1557
#include "defer-friend-function-def.h"
class Bug1557_QVariant {};
void foo() {
  Bug1557_QList<Bug1557_QVariant>().at;
  Bug1557_qt_getQtMetaObject();
}
