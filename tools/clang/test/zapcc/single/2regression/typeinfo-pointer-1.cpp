// RUN: %zap_compare_object
// Bug 1617
struct Flags {
  virtual ~Flags();
};
struct ModelPart : Flags {
  ~ModelPart();
};
namespace std {
struct type_info {
  type_info(type_info const &);
};
void foo1() {
  type_info(typeid((ModelPart *)nullptr));
}
}
