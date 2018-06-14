#include "f.h"
int InlinedFunctionWasRemovedFoo();
int InlinedFunctionWasRemovedStruct::foo() { return InlinedFunctionWasRemovedFoo(); }
void InlinedFunctionWasRemovedStruct::bar() { foo(); }
