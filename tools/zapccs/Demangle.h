// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCC_DEMANGLE_H
#define ZAPCC_DEMANGLE_H

#include "llvm/ADT/StringRef.h"

std::string demangle(llvm::StringRef name);

#endif // ZAPCC_DEMANGLE_H
