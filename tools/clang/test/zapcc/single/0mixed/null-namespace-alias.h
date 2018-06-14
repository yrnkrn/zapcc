#pragma once
struct NullNamespaceAliasStruct {};
template <int> class NullNamespaceAlias {
  void bar();
  NullNamespaceAliasStruct s;
};
