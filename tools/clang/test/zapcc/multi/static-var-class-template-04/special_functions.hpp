template <class> struct Bug1788_bessel_j0_initializer {
  static int initializer;
};
template <class T> int Bug1788_bessel_j0_initializer<T>::initializer;
template <class> int Bug1788_airy_ai() {
  Bug1788_bessel_j0_initializer<int>::initializer;
}
