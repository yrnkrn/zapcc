// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 147
#include "missing-template-static.h"
template <typename T> int MissingTemplateStatic<T>::Var;
int main() {
  return MissingTemplateStatic<char>::Var;
}
