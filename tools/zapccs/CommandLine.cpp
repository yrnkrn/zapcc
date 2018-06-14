// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "CommandLine.h"
#include "StringConst.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

void resetNumOccurrencesFlags() {
  auto &RegisteredOptions =
      llvm::cl::getRegisteredOptions(*llvm::cl::TopLevelSubCommand);
  for (auto &RegisteredOption : RegisteredOptions) {
    llvm::cl::Option *Opt = RegisteredOption.second;
    Opt->setNumOccurrences(0);
  }
}

bool DebugTimeFlag = false;

#ifndef NDEBUG
// Return true if the argument is a debug argument.
static bool debugFlagType(const llvm::StringRef Arg) {
  return (Arg == DebugSwitch) || Arg.startswith(DebugOnlySwitch) ||
         (Arg == DebugTimeSwitch);
}

void parseDebugFlags(llvm::ArrayRef<const char *> Argv) {
  llvm::DebugFlag = false;
  DebugTimeFlag = false;
  llvm::setCurrentDebugTypes(nullptr, 0);
  llvm::SmallVector<const char *, 8> DebugArgv;
  // cl::parseCommanLineOptions skips the first argument.
  DebugArgv.push_back("");
  for (const char *Arg : Argv)
    if (debugFlagType(Arg))
      DebugArgv.push_back(Arg);

  if (DebugArgv.size() < 2)
    return;
  DebugArgv.push_back(nullptr);
  llvm::cl::ParseCommandLineOptions(DebugArgv.size() - 1, DebugArgv.data());
}
#else
void parseDebugFlags(llvm::ArrayRef<const char *>) {}
#endif
