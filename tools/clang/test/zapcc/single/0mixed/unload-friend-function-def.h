#pragma once
#include "unload-friend-function-def-2.h"
template <typename T> struct Bug1557QList {
  T at;
  friend void *Bug1557qt_getQtMetaObject() {}
};
class Bug1557QVariant;
