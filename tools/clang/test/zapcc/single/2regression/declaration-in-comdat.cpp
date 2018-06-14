// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 316
struct Base1 {
  virtual void foo();
};
struct Base2 {
  virtual ~Base2();
};
struct Derived : virtual Base1, Base2 {
  virtual void bar();
  void foo();
};
void Derived::bar() {}
