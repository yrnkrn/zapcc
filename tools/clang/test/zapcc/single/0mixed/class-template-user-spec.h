#pragma once
template <typename> struct Bug1909FullMatrix {
  template <typename number2> void fill(Bug1909FullMatrix<number2>){};
};
