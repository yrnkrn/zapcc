#include "f.h"
template <typename T> class MultiIfriend {};
template <typename T> struct MultiIson;
template <> struct MultiIson<MultiIself> : public MultiIfriend<MultiIself> {};
