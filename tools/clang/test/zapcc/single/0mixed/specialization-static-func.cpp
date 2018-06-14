// RUN: %zap_compare_object
#include "specialization-static-func.h"
template <> void SpecializationStaticFunction<char>::foo() {
}
int main() { SpecializationStaticFunction<char>::foo(); }
