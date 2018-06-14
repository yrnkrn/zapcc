// RUN: %zap_compare_object CXXFLAGS="-O"
// Bug 1855
struct t {
  ~t() {}
} t_instance;
