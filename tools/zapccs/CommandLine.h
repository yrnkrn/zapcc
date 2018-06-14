// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef TOOLS_ZAPCC_COMMANDLINE_H_
#define TOOLS_ZAPCC_COMMANDLINE_H_

#include "llvm/ADT/ArrayRef.h"

void resetNumOccurrencesFlags();
void parseDebugFlags(llvm::ArrayRef<const char *> Argv);
extern bool DebugTimeFlag;

#endif
