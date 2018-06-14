// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCCS_FDMAPPING_H
#define ZAPCCS_FDMAPPING_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallPtrSet.h"

namespace clang {
class Decl;
class FileEntry;

class FileDeclsMapping {
  typedef llvm::DenseMap<const FileEntry *, llvm::SmallPtrSet<const Decl *, 8>>
      FileDeclsMap;
  FileDeclsMap FileDecls;
  typedef llvm::DenseMap<const Decl *, const FileEntry *> DeclFileMap;
  DeclFileMap DeclFiles;

public:
  FileDeclsMap::iterator find(const FileEntry *FE) {
    return FileDecls.find(FE);
  }
  FileDeclsMap::iterator file_end() { return FileDecls.end(); }
  DeclFileMap::iterator find(const Decl *D) { return DeclFiles.find(D); }
  DeclFileMap::iterator begin() { return DeclFiles.begin(); }
  DeclFileMap::iterator end() { return DeclFiles.end(); }
  void erase(const Decl *D) {
    auto I = DeclFiles.find(D);
    if (I == DeclFiles.end())
      return;
    const FileEntry *FE = I->second;
    auto J = FileDecls.find(FE);
    assert(J != FileDecls.end());
    J->second.erase(D);
    DeclFiles.erase(I);
  }
  void erase(const FileEntry *FE) {
    auto I = FileDecls.find(FE);
    if (I == FileDecls.end())
      return;
    for (const Decl *D : I->second)
      DeclFiles.erase(D);
    FileDecls.erase(I);
  }
  void insert(const Decl *D, const FileEntry *FE) {
    assert(D);
    assert(FE);
    erase(D);
    FileDecls[FE].insert(D);
    DeclFiles[D] = FE;
  }
};

} // namespace clang

#endif
