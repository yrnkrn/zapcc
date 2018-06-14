// RUN: %zap_compare_object
template <typename> struct Bug735reverseIterator { int i; };
template <typename Iterator>
auto Bug735niter_base(Bug735reverseIterator<Iterator> it)
    -> decltype(Bug735niter_base(it.i)) {
  Bug735niter_base(it.i);
}
template <typename Iterator> void Bug735niter_base(Iterator);
void fn1() { Bug735niter_base(Bug735reverseIterator<int>()); }
