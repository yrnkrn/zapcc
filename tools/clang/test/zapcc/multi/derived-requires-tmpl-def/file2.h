#pragma once
#include "file1.h"
namespace Bug702dealii {
template <typename T> struct FullMatrix : Table<T> {};
}
