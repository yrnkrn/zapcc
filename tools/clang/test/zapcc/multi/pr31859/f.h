template <class> struct Bug1858col_;
template <class> struct Bug1858vec_traits;
template <class OriginalMatrix>
struct Bug1858vec_traits<Bug1858col_<OriginalMatrix>> {
  template <int> static void write_element();
};
