// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ZAPCC_USERDATA_H
#define ZAPCC_USERDATA_H

#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallPtrSet.h"

namespace clang {
class Decl;
class FileEntry;
class NestedNameSpecifier;
class Type;

struct UserData {
  llvm::SetVector<const Decl *> Decls;
  llvm::SmallPtrSet<Type *, 8> Types;
  // Types that need new canonical since their canonical was unloaded.
  llvm::SmallPtrSet<Type *, 8> RecanonTypes;
  llvm::SmallPtrSet<NestedNameSpecifier *, 8> NestedNameSpecifiers;
  llvm::SetVector<const FileEntry *> FileEntries;
  unsigned MutableFileEntries;
  UserData() { clear(); }
  void clear() {
    Decls.clear();
    Types.clear();
    RecanonTypes.clear();
    NestedNameSpecifiers.clear();
    FileEntries.clear();
    MutableFileEntries = 0;
  }
};
}

#endif
