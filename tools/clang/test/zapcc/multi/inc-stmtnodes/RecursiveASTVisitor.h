template <typename> struct Bug1860RecursiveASTVisitor {
  bool TraverseDecl() { WalkUpFromBinaryOperator(); }
#include "StmtNodes.inc"
};
