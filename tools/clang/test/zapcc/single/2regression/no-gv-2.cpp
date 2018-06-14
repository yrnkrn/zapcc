// RUN: %zap_compare_object
// RUN: %zap_compare_object
struct base {
  virtual ~base() {}
};
int main() { 
  new base;
}
