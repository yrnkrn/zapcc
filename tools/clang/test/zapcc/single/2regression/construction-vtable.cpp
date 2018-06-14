// RUN: %zap_compare_object CXXFLAGS="-O1"
// Bug 1219
struct e {
  virtual ~e();
};
struct g : virtual e {
  void f();
};
struct istrstream : g {
  istrstream() { f(); }
  ~istrstream();
};
void foo1() { istrstream(); }
