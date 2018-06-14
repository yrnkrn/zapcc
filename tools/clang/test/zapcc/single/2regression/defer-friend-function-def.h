#include "defer-friend-function-def-2.h"
template <typename T> struct Bug1557_QList {
  T at;
  class const_iterator;
};
class Bug1557_QVariant;
typedef Bug1557_QList<Bug1557_QVariant> Bug1557_QVariantList;
class Bug1557_QObject {
  friend void *Bug1557_qt_getQtMetaObject() {}
};
