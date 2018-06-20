// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ZAPCC_DEMANGLE_H
#define ZAPCC_DEMANGLE_H

#include "llvm/ADT/StringRef.h"

std::string demangle(llvm::StringRef name);

#endif // ZAPCC_DEMANGLE_H
