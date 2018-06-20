// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ZAPCC_USERREPLACER_H
#define ZAPCC_USERREPLACER_H

#include "llvm/ADT/SetVector.h"
#include "llvm/Support/Casting.h"

namespace clang {
class Decl;
class Expr;
class NestedNameSpecifier;
class Type;
class Stmt;

class UserReplacer {
public:
  UserReplacer(Decl *Dependee, Decl *Replacement)
      : Dependee(Dependee), Replacement(Replacement), Replaced(false) {}
  void replaceDecl(Decl *D);
  void replaceType(Type *T);
  void replaceNestedNameSpecifier(NestedNameSpecifier *NNS);
  bool wasReplaced() { return Replaced; }

private:
  UserReplacer(UserReplacer &) = delete;
  UserReplacer &operator=(UserReplacer &) = delete;
  Decl *Dependee;
  Decl *Replacement;
  bool Replaced;

  void replaceStmt(Stmt *S);
  void replaceExpr(Expr *E);
  template <typename T> T *nonUser(T *D) {
    if (D != Dependee && D != Replacement)
      return D;
    Replaced = true;
    return llvm::cast<T>(Replacement);
  }
};
}

#endif
