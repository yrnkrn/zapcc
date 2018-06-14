#include "file1.h"
template <int, int> class Bug500FECollection;

#ifndef dealii__fe_collection_h
#define dealii__fe_collection_h

  template <int dim,int spacedim = dim> class Bug500FECollection : Bug500Subscriptor {
    class : Bug500Quadrature<0> {};
  };

#endif
template <int, int> class Bug500FECollection;
