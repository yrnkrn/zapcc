#pragma once
#include "file2.h"
template <class T> struct Bug446ThreadSafeRefCounted {
  void deref() { delete static_cast<T *>(this); }
};
struct Bug446Database : public Bug446ThreadSafeRefCounted<Bug446Database> {
  ~Bug446Database();
};
struct Bug446ScriptExecutionContext {
  Bug446Database m_databaseContext;
};
struct Bug446Document : Bug446ScriptExecutionContext {};
Bug446Document &document();
inline void Bug446Node::scriptExecutionContext() {
  Bug446ScriptExecutionContext *S = &document();
  (void)S;
}
