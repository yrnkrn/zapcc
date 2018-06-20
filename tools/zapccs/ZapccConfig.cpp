// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ZapccConfig.h"
#include "TextFile.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include <fstream>

namespace clang {

static bool readSections(std::ifstream &ConfigFile,
                         ZapccConfig::AllPatternsType &AllPatterns) {
  ZapccConfig::PatternsType *Patterns = nullptr;
  std::string Line;
  while (std::getline(ConfigFile, Line)) {
    if (!Line.size() || Line[0] == '#' || Line[0] == ';')
      continue;
    if (Line[0] == '[')
      Patterns = &AllPatterns[Line];
    else if (Patterns)
      Patterns->push_back(Line);
  }
  return AllPatterns.size();
}

static unsigned setNumeric(llvm::StringRef Value, unsigned Limit) {
  unsigned V;
  if (Value.getAsInteger(10, V)) {
    llvm::errs() << "'" << Value << "' is not a number, set to 1024.\n";
  } else if (V > Limit) {
    V = Limit;
    llvm::errs() << Value << "' is too high, set to " << Limit << ".\n";
  }
  return V;
}

bool ZapccConfig::readConfigFile(llvm::StringRef ExePath) {
  llvm::SmallString<1024> ConfigName(ExePath);
  llvm::sys::path::replace_extension(ConfigName, "config");
  std::ifstream ConfigFile(ConfigName.str());
  if (!ConfigFile) {
    llvm::errs() << "error: configuration file '" << ConfigName
                 << "' was not found.\n";
    return false;
  }
  if (!readSections(ConfigFile, AllPatterns))
    return false;
  PatternsType &Memory = getPatterns("[MaxMemory]");
  if (Memory.size() != 1) {
    llvm::errs() << "MaxMemory section should have one entry only.\n";
    return false;
  }
  MaxMemory = setNumeric(Memory[0], 16384);
  return true;
}

ZapccConfig::PatternsType &ZapccConfig::getPatterns(llvm::StringRef Section) {
  auto I = AllPatterns.find(Section);
  if (I == AllPatterns.end())
    llvm::errs() << Section << " not found!\n";
  return I->getValue();
}
}
