template <typename> struct Bug1924QMetaTypeId {
  static int qt_metatype_id() {}
};
template <typename T> void Bug1924QMetaTypeId2() { Bug1924QMetaTypeId<T>::qt_metatype_id(); }
