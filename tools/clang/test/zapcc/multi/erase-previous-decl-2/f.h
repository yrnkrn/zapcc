#pragma once
struct Bug325SimpleStruct {
  Bug325SimpleStruct();
};
template <typename> struct Bug325Template {
  void foo() { 
    Bug325SimpleStruct v;
  }
};
struct Bug325Using {
  Bug325Template<void> AbbrDeclSets;
  void extract();
};
