// RUN: %zap_compare_object
// Bug 323
#include "dependentsizedarray.h"
template <int dim> void DependentSizedArrayFunc() { DependentSizedArray<dim> a[dim]; }
int main() {
}
