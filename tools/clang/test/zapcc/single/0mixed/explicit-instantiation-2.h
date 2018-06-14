#pragma once

struct ilist_traits;

struct Instruction {
  static ilist_traits Instruction::*getSublistAccess() { return 0; }
};

template <typename ItemParentClass>
struct SymbolTableListTraits {
  void getListOwner() { auto a = &((ItemParentClass *)nullptr->*ItemParentClass::getSublistAccess()); }
};

struct ilist_traits : public SymbolTableListTraits<Instruction> {};
