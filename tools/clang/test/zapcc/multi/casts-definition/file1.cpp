#include "f.h"
struct Bug427ScriptExecutionContext {};
struct Bug427Document : Bug427ScriptExecutionContext {
  void foo() { Bug427downcast<Bug427Document>(s); }
  Bug427ScriptExecutionContext s;
};
