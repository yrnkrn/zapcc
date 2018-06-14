#pragma once
#include "qmetatype.h"
template <> struct Bug1924QMetaTypeId<int> {
  static int qt_metatype_id() {}
};
