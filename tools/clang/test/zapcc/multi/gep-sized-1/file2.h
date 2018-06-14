#pragma once
class aa *Bug950LLVMTy;
int Bug950getExtendedVectorNumElements();
struct Bug950EVT {
  int getVectorNumElements() { return Bug950getExtendedVectorNumElements(); }
};
