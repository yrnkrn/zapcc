// RUN: %zap_compare_object
#include "struct-struct-static-user-func.h"
template <class T>
void UserFunction() {
}
int main() {
  WrapperStruct::CUF.JustFunc();
}
