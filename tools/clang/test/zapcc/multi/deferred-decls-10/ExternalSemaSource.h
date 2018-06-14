#pragma once
struct Bug1641SmallVectorImpl {
  ~Bug1641SmallVectorImpl();
};
struct Bug1641SmallVector : Bug1641SmallVectorImpl {};
template <class _Tp> struct Bug1641pair {
  int first;
  _Tp second;
};
struct Bug1641LateParsedTemplate;
template <typename _Tp> void Bug1641DeleteContainerSeconds() {
  Bug1641pair<_Tp> *I = 0;
  delete I->second;
}
