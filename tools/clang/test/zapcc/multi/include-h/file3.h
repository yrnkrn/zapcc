#ifndef dealii__tria_objects_h
#define dealii__tria_objects_h
template <int> struct Bug809GeometryInfo;
template <typename> class Bug809TriaObjects {
  void reserve_space();
  void a(int b) { b << c; }
  int c;
};
#endif
