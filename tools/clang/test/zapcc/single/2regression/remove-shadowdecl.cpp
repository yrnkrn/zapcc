// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 175
struct RemoveShadowdeclBase {
  void foo();
};
struct RemoveShadowdeclDerived : RemoveShadowdeclBase {
  using RemoveShadowdeclDerived::RemoveShadowdeclBase::foo;
  void foo();
};
int main() {
}
