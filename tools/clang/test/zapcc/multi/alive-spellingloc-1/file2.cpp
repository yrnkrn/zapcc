#include "DeclVisitor.h"
void foo2() { Bug1770Base<int>().VisitExternCContextDecl(); }
