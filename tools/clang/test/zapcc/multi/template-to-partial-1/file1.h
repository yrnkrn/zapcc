#pragma once
#include "file2.h"
#include "empty.def"
template <typename> class Bug1090CVRecord;
template <typename a> struct Bug1090VarStreamArrayExtractor<Bug1090CVRecord<a>> {};
