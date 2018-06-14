// RUN: %zap_compare_object CXXFLAGS="-fsanitize=address"
// Bug 1065
struct b {
  b();
} a;
