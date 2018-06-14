// RUN: %zap_compare_object
// Bug 1591
namespace std {
struct type_info {
  void operator==(type_info) const;
};
}
class ORealDynamicLoader {
  virtual ~ORealDynamicLoader();
};
void getORealDynamicLoaderTypeInfo() {
  typeid(ORealDynamicLoader *) == typeid(int *);
}
