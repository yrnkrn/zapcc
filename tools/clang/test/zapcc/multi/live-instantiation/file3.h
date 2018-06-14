#pragma once
struct Bug530FunctionPass {
  virtual void getAnalysisUsage();
};
template <typename PassName> void Bug530callDefaultCtor() { PassName(); }
