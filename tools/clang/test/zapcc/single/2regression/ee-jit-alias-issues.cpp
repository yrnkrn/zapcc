// RUN: %zap_compare_object
// RUN: %zap_compare_object
class EE_JIT_C {
public:
  virtual ~EE_JIT_C();
};
EE_JIT_C::~EE_JIT_C() {}
int main() {
  EE_JIT_C c;
}
