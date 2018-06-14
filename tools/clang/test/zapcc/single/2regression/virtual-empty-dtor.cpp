// RUN: %zap_compare_object
// RUN: %zap_compare_object
struct BaseClass {
  ~BaseClass();
};
struct DerivedClass : virtual BaseClass {
  ~DerivedClass();
};
BaseClass::~BaseClass() {}
DerivedClass::~DerivedClass() {}
int main() {  
  DerivedClass D;
}
