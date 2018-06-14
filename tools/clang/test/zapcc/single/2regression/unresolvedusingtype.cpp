// RUN: %zap_compare_object
// RUN: %zap_compare_object
template <class T>
struct Base {
  typedef int foo;
};
template <class T>
struct Derived : Base<T> {
  using typename Base<T>::foo;
  foo f;
};
int main() {
  Derived<int> D;
  D.f = 3;
}
