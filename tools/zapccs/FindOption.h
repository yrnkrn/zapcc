// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCC_FINDOPTION_H
#define ZAPCC_FINDOPTION_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

// Return option index if found or -1 if not found.
int findOption(llvm::ArrayRef<const char *> Argv, llvm::StringRef Opt);
// Return:
// -1 if missing or invalid server id after the switch,
// server id value if valid numeric value.
int getIntOption(llvm::ArrayRef<const char *> Argv, int Index);

#endif //  ZAPCC_FINDOPTION_H
