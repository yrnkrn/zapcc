// RUN: %zap_compare_object
// Bug 234
// Bug 237
//extern "C"
#include "user-linkagespec-system-func.h"
UserLinkageSpecStruct::UserLinkageSpecStruct() {}
int main() { 
}
