#include "full_matrix.h"
namespace {
template class Bug1909_FullMatrix<double>;
template void Bug1909_FullMatrix<double>::fill(Bug1909_FullMatrix);
}
