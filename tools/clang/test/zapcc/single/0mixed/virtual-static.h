#pragma once
template <class T> struct MainTemplate {
  virtual ~MainTemplate() { }
  virtual void SomeFunc() { T().OtherFunc(); }
};
struct MixedStruct {
  void OtherFunc();
};
