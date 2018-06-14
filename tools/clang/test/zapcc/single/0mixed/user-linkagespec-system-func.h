#pragma once
void UserLinkageSpecFunc() {}
template <typename> struct UserLinkageSpecTemplate {
  virtual void foo();
};
struct UserLinkageSpecStruct : UserLinkageSpecTemplate<int> {
  void foo() { UserLinkageSpecFunc(); }
  UserLinkageSpecStruct();
};
