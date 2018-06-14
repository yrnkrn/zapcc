// RUN: %zap_compare_object
// RUN: %zap_compare_object
template <typename> class ClassWithDeclType {
  template <typename T> auto foo(T t) -> decltype(t);
};
class C;
extern "C" int puts(const char *);
int main() { 
  puts("class-with-decltype");
  ClassWithDeclType<C> a;
}
