#include "f.h"
template <int dim> int i;
template <int dim> EmptyOverloadExpr<dim>::EmptyOverloadExpr() { n = i<dim>; }
