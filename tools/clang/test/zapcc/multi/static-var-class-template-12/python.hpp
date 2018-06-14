template <class T> struct Bug1942registered_base { static T converters; };
template <class T> T Bug1942registered_base<T>::converters;
template <class T> struct Bug1942make_instance {
  static void execute() { Bug1942registered_base<T>::converters; }
};
template <class Conversion> struct Bug1942to_python_converter {
  Bug1942to_python_converter() { &Conversion::convert; }
};
template <class MakeInstance> struct Bug1942class_cref_wrapper {
  Bug1942to_python_converter<
      Bug1942class_cref_wrapper<Bug1942make_instance<MakeInstance>>>
      x;
  void convert() { MakeInstance::execute(); }
};
