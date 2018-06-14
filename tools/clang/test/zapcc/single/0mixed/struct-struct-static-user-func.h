#pragma once
template <class T> void UserFunction();
struct WrapperStruct {
  struct CallUserFunctionStruct {
    CallUserFunctionStruct() { UserFunction<int>(); }
    void JustFunc() {}
  };
  static CallUserFunctionStruct CUF;
};
WrapperStruct::CallUserFunctionStruct WrapperStruct::CUF;
