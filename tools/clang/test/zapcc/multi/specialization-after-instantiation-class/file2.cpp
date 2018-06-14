#include "file1.h"
template <typename T> class SpecializationAfterInstantiationClass {};
template <> class SpecializationAfterInstantiationClass<int> {};
