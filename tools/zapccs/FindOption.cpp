// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "FindOption.h"
#include "StringConst.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

#include <cstring>

int findOption(llvm::ArrayRef<const char *> Argv, llvm::StringRef Opt) {
  auto I = std::find_if(Argv.begin(), Argv.end(),
                        [=](const char *s) { return s == Opt; });
  if (I != Argv.end())
    return I - Argv.begin();
  return -1;
}

int getIntOption(llvm::ArrayRef<const char *> Argv, int Index) {
  assert(Argv.size());
  if (Index < 0 || Index >= int(Argv.size())) {
    llvm::errs() << "Illegal Index.\n";
    return -1;
  }
  llvm::StringRef StringValue = Argv[Index];
  int Value;
  if (StringValue.getAsInteger(10, Value)) {
    llvm::errs() << "Invalid server id '" << StringValue << "'\n.";
    return -1;
  }
  return Value;
}
