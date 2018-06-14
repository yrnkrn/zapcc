#pragma once
struct StructTemplatedStruct {
  template <class T> void Visit();
  void callTemplate() {
    Visit<int>();
  }
};
