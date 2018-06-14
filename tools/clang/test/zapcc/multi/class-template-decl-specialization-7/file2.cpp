#include "itkNumericTraits.h"
void foo() {
  Bug2343NumericTraits<Bug2343vector<float>>::GetLength();
}
