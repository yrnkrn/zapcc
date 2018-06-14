#pragma once
struct Bug2014scoped_Bug2014Array {
  int px[2];
  Bug2014scoped_Bug2014Array();
};
struct Bug2014Array {
  Bug2014Array(const Bug2014Array &from) {
    data_ = from.data_;
    n_ = from.n_;
  }
  Bug2014scoped_Bug2014Array data_;
  long n_;
};
struct Bug2014Disposable : Bug2014Array {};
Bug2014Disposable foo1();
struct Bug2014TridiagonalOperator {
  Bug2014TridiagonalOperator();
  int lowerDiagonal_[8];
};
void Bug2014foo2() {
  Bug2014Array low = foo1();
  Bug2014TridiagonalOperator result0;
}
