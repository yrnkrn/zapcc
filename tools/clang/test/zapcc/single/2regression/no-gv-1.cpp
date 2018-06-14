// RUN: %zap_compare_object
// RUN: %zap_compare_object
struct base {
  base() { whoami(); }
  virtual ~base() {}
  virtual void whoami() {}
};
int main() { 
  new base;
}
