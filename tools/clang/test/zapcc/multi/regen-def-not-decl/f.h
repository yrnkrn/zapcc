#pragma once
struct Bug497MDNode {
  void deleteTemporary();
};
struct Bug497A {
  void foo(Bug497MDNode *p1);
};
void Bug497A::foo(Bug497MDNode *p1) { p1->deleteTemporary(); }
