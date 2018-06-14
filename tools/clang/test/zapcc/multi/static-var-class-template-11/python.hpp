template <class T> struct Bug1944registered_base { static T converters; };
template <class T> T Bug1944registered_base<T>::converters = T();
template <class MakeInstance> struct Bug1944class_cref_wrapper {
  Bug1944class_cref_wrapper();
};
template <class MakeInstance>
Bug1944class_cref_wrapper<MakeInstance>::Bug1944class_cref_wrapper() {
  Bug1944registered_base<MakeInstance>::converters;
}
