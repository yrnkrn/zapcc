// RUN: %zapccxx %s -c -o %t | tee %t.txt || FileCheck %s -check-prefix=CHECK-COMPILE -input-file=%t.txt --allow-empty
// CHECK-COMPILE-NOT: note: system {{.*}} was removed
// Do not check for error or warning, this does not compile.
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
template <typename> struct StructBug43;
struct UndefinedStructBug43;
template <> struct StructBug43<char> { typedef UndefinedStructBug43 type; };
template <typename T> struct BaseBug43 {
  virtual ~BaseBug43();
  virtual typename StructBug43<T>::type Func1() {}
};
struct DerivedBug43 : BaseBug43<char> {
  ~DerivedBug43();
};
DerivedBug43::~DerivedBug43() {}
