#pragma once
struct BaseStructVT {
  BaseStructVT();
  virtual ~BaseStructVT() {}
};
struct DerivedStructVT : BaseStructVT {};
