template <typename> struct Bug1886function1 {
  static void foo() {}
};
struct Bug1886function : Bug1886function1<int> {};
