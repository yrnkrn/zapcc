// Copyright (c) 2014-2018 Ceemple Software Ltd. All rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ZAPCC_REPLACEDECLINMAP_H
#define ZAPCC_REPLACEDECLINMAP_H

namespace clang {
class DeclContext;
class NamedDecl;
bool ReplaceDeclInMapImpl(NamedDecl *OriginalND, NamedDecl *ReplacementND,
                          DeclContext *DC);
}
#endif
