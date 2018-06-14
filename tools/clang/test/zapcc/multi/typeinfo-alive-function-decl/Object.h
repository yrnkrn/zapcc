#pragma once
#include "Var.h"
namespace std {
class type_info;
}
struct Bug1927Object {
  ~Bug1927Object();
};
template <typename T> struct Bug1927VarHolderImpl {
  Bug1927VarHolderImpl(T) { typeid(T); }
};
