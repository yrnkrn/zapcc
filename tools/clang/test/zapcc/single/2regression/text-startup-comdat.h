class Context;
struct Context {
  static Context default_;
  Context() {}
};
__attribute__((weak)) Context Context::default_;
