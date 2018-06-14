// RUN: %zap_compare_object CXXFLAGS="-O" 
// RUN: %zap_compare_object CXXFLAGS="-O" 
// Bug 1529
#include "template-def-replaces-decl.h"
namespace Bug1529llvm {
extern template class Bug1529DomTreeNodeBase<int>;
Bug1529unique_ptr<Bug1529DomTreeNodeBase<int>> P;
}
template class Bug1529llvm::Bug1529DomTreeNodeBase<int>;
