// RUN: %zap_duplicate
// Bug 114
#define DEFINE_STDCXX_CONVERSION_FUNCTIONS(x)                 \
  inline void *unwrap(void *P) { return P; }                  \
  template <typename T> void *unwrap(T *P) { void *Q = P; return Q; }
DEFINE_STDCXX_CONVERSION_FUNCTIONS(0)
#undef DEFINE_STDCXX_CONVERSION_FUNCTIONS
int main() {
  unwrap(nullptr);
}
