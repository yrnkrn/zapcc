#include "RecursiveASTVisitor.h"
void transmakeAssignARCSafe() {
  Bug1860RecursiveASTVisitor<int> assignCheck;
  assignCheck.TraverseDecl();
}
