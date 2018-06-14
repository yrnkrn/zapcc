// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include <string.h>

const char *findBaseName(const char *FullName) {
  const char *BaseName = FullName + strlen(FullName);
  while (BaseName > FullName) {
    if ((*BaseName == '\\') || (*BaseName == '/'))
      return BaseName + 1;
    --BaseName;
  }
  return BaseName;
} // findBaseName
