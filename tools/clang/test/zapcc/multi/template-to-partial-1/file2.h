#include "file3.h"
template <typename> struct Bug1090VarStreamArrayExtractor {
  Bug1090conditional<Bug1090c<false>::value> cd;
};
template <typename b, typename = Bug1090VarStreamArrayExtractor<b>>
class Bug1090VarStreamArray {
  Bug1090VarStreamArrayExtractor<b> d;
};
