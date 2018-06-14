#pragma once
template <class> struct MarkDeclUsed1 { static int type; };

struct MarkDeclUsed2 {
  void isNull() { foo<void *>(); }
  template <class T> void foo() { (void)MarkDeclUsed1<T>::type; }
};
