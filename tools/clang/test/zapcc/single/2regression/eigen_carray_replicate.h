#pragma once
extern "C" int puts(const char *);

template <typename> struct ECR_B;
template <typename T> struct ECR_C {
  T derived() { return *static_cast<T *>(this); }
  template <typename X> bool isApprox( ECR_C<X> &) ;
  template <typename X> ECR_B<X> operator-(X p1) {
    T b;
    return ECR_B<X>(p1);
  }
};
struct ECR_A : ECR_C<ECR_A> {
  int value;
  int rows() { return value; }
};
template <typename Rhs> struct ECR_B : ECR_C<ECR_B<Rhs>> {
  ECR_B(Rhs p2) : m_rhs(p2) {}
  ECR_A m_lhs;
  Rhs &m_rhs;
  int rows() { return m_rhs.rows(); }
};

struct ECR_P : ECR_C<ECR_P> {
  ECR_P();
  ECR_A &m_matrix;
  int rows()  { return m_matrix.rows(); }
};

template <typename T> struct ECR_K {
  static int run(T p1) {
    if (p1.rows())
      puts(__PRETTY_FUNCTION__);
    return 0;
  }
};

struct ECR_L {
  static bool run(ECR_A p1, ECR_P p2) {
    ECR_P a(p2);
    ECR_K<ECR_B<ECR_P>>::run((p1 - p2));
    ECR_K<ECR_P>::run(a);
  }
};

