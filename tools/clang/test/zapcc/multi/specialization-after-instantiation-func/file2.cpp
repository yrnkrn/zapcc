#include "file1.h"
template <typename T> void SpecializationAfterInstantiationFunc() {}
template <> void SpecializationAfterInstantiationFunc<int>() {}
