struct Bug1884cpp_int_backend {};
void Bug1884boo(const Bug1884cpp_int_backend &);
bool Bug1884aaa;
template <class T, class U> void Bug1884eval_bitwise_and1(const T &, const U &);
template <class T> void Bug1884eval_bitwise_and_default(T t) {
  Bug1884eval_bitwise_and1(t, t);
}
template <class T, class U>
inline void Bug1884eval_bitwise_and1(const T &, const U &) {
  if (Bug1884aaa)
    Bug1884boo(Bug1884cpp_int_backend());
  Bug1884boo(Bug1884cpp_int_backend());
  Bug1884boo(Bug1884cpp_int_backend());
  Bug1884boo(Bug1884cpp_int_backend());
  Bug1884boo(Bug1884cpp_int_backend());
  Bug1884boo(Bug1884cpp_int_backend());
  Bug1884boo(Bug1884cpp_int_backend());
}
