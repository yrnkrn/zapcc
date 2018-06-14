template <class> struct Bug1788_cessel_j0_initializer {
  struct init {
    init() {}
  } static initializer;
};
template <class T>
typename Bug1788_cessel_j0_initializer<T>::init
    Bug1788_cessel_j0_initializer<T>::initializer;
template <class> int Bug1788_ciry_ai() {
  Bug1788_cessel_j0_initializer<int>::initializer;
}
