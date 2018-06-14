#pragma once
template <typename T> struct VectorDependentStruct {
  VectorDependentStruct() { T(); }
};
struct VectorDependentStructHelper;
struct VectorDependentStructContainer {
  VectorDependentStruct<VectorDependentStructHelper> v;
};
