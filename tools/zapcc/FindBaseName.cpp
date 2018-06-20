// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
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
