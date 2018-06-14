namespace {
template <typename> struct Bug1909_FullMatrix {
  template <typename number2> void fill(Bug1909_FullMatrix<number2>){};
};
}
