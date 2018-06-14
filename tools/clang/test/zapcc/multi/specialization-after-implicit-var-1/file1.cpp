#include "f.h"
template <typename T> int SpecializationAfterImplicit1<T>::value;
int foo() { 
  return SpecializationAfterImplicit1<char>::value;
}
