#pragma once
struct dominate_A {
  double d[2];
};
struct dominate_B {
  double d[3];
};
inline int dominate_fn1() { return 0; }

inline int dominate_fn2(dominate_A p1) {
  if (dominate_fn1())
    return 0;
  dominate_fn2(p1);
}

void dominate_fn3(dominate_A &);
void dominate_fn4(dominate_B);
inline int dominate_fn5() {
  {
    dominate_A a, b;
    dominate_fn3(b);
    a.d[0] = b.d[0];
    a.d[1] = b.d[1];
    dominate_fn2(a);
  }
  dominate_B c;
  dominate_fn4(c);
}
