#include "Value.h"
template <typename> struct Bug2157SymbolTableListTraits {};
struct Bug2157AttributeSetImpl;
struct Bug2157AttributeSet {
  Bug2157AttributeSetImpl *pImpl;
};
void Bug2157getOrInsertFunction(Bug2157AttributeSet, void *...);
Bug2157SymbolTableListTraits<int> getSublistAccess0;
