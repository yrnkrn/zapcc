#pragma once
#include "file2.h"
template <typename> class Bug950ArrayRef;
void Bug950dd(Bug950ArrayRef<int>);
struct Bug950VectorType {
  int a;
};
int Bug950getExtendedVectorNumElements() { return ((Bug950VectorType *)Bug950LLVMTy)->a; }
