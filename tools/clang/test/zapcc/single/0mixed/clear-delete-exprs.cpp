// RUN: %zap_compare_object
// Bug 290
#include "clear-delete-exprs.h"
class TestClass290 {};
int main() { 
  ClearDeleteExprs<TestClass290> p;
}
