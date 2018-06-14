template <class _CharT, class = _CharT> class Bug1549c_basic_string;
template <class> class Bug1549c___wrap_iter {
  template <class, class> friend class Bug1549c_basic_string;
};
Bug1549c___wrap_iter<int> Bug1549c_begin1();
