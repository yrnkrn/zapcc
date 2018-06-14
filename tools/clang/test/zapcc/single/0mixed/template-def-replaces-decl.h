#pragma once
template <typename T> struct Bug1529unique_ptr {
  T *p;
  Bug1529unique_ptr() { delete p; }
};
namespace Bug1529llvm {
template <class> class Bug1529DomTreeNodeBase {};
}
