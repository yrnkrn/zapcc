#pragma once
struct StructVirtualBase {
  virtual void Func();
};
struct StructVirtualDerived : StructVirtualBase {};
