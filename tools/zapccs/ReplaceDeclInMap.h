// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#ifndef ZAPCC_REPLACEDECLINMAP_H
#define ZAPCC_REPLACEDECLINMAP_H

namespace clang {
class DeclContext;
class NamedDecl;
bool ReplaceDeclInMapImpl(NamedDecl *OriginalND, NamedDecl *ReplacementND,
                          DeclContext *DC);
}
#endif
