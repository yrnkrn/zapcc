template <class> struct Bug1770Base {
#define DECL void VisitExternCContextDecl() {}
#include "DeclNodes.inc"
};
