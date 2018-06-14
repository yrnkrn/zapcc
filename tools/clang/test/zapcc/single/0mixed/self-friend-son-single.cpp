// RUN: %zap_compare_object
#include "self-friend-son-single.h"
template <typename T> class SingleIfriend {};
template <typename T> struct SingleIson;
template <> struct SingleIson<SingleIself> : public SingleIfriend<SingleIself> {};
