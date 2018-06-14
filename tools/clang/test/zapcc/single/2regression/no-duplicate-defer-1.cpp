// RUN: %zapccxx %s -c -o %t | tee %t.txt && FileCheck %s -check-prefix=CHECK-COMPILE -input-file=%t.txt --allow-empty
// CHECK-COMPILE-NOT: note: system {{.*}} was removed
// CHECK-COMPILE-NOT: error: 
// CHECK-COMPILE-NOT: Assertion
// CHECK-COMPILE-NOT: Stack dump:
template <typename> struct StructBug42 {};
template <typename T> struct BaseBug42 {
  struct StructInner {
    ~StructInner();
  } S;
  virtual ~BaseBug42() {}
  virtual StructBug42<T> JustFunc1() {}
};
struct DerivedBug42 : BaseBug42<char> {
  ~DerivedBug42();
};
DerivedBug42::~DerivedBug42() {}
