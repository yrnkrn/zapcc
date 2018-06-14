// Copyright (c) 2014-2017 Ceemple Software Ltd. All rights Reserved.
#include "clang/AST/DeclContextInternals.h"

namespace clang {

bool ReplaceDeclInMapImpl(NamedDecl *OriginalND, NamedDecl *ReplacementND,
                          DeclContext *DC) {
  DeclarationName NDName = OriginalND->getDeclName();
  // Anonymous namespaces has no name.
  if (!NDName)
    return false;
  StoredDeclsMap *Map = DC->getLookupPtr();
  if (!Map)
    return false;
  StoredDeclsMap::iterator Pos = Map->find(NDName);
  // Method function template specializations are not added to any
  // DeclContext.
  // Maybe this is a bug. See TemplateDeclInstantiator::VisitCXXMethodDecl()
  // comment: "If there's a function template, let our caller handle it."
  // http://llvm.org/bugs/show_bug.cgi?id=22842
  if (Pos == Map->end())
    return false;
  StoredDeclsList &SDL = Pos->second;
  // OriginalND may not in SDL. This can happen if we are father
  // of the real decl, for example OriginalND is FunctionDecl but
  // SDL has TemplateFunctionDecl.
  if (SDL.isNull())
    return false;
  if (Decl *SDL1 = SDL.getAsDecl()) {
    if (SDL1 != OriginalND)
      return false;
    if (ReplacementND)
      SDL.setOnlyValue(ReplacementND);
    else
      SDL.remove(OriginalND);
  } else {
    StoredDeclsList::DeclsTy &SDLV = *SDL.getAsVector();
    StoredDeclsList::DeclsTy::iterator I =
        std::find(SDLV.begin(), SDLV.end(), OriginalND);
    if (I == SDLV.end())
      return false;
    // Avoid duplicates after replacement.
    if (ReplacementND)
      if (llvm::any_of(SDLV,
                       [=](NamedDecl *ND) { return ND == ReplacementND; }))
        ReplacementND = nullptr;
    if (ReplacementND) {
      (*I) = ReplacementND;
    } else {
      SDLV.erase(I);
#if 0
      if (SDLV.size() == 1) {
        NamedDecl *ND = SDLV[0];
        delete SDL.getAsVector();
        SDL.Data = (NamedDecl *)ND;
      }
#endif
    }
  }
  return true;
}

} // namespace clang
