struct Bug1581a_regex_error {
  Bug1581a_regex_error(int);
};
template <class> struct Bug1581a_basic_regex_implementation {
  Bug1581a_basic_regex_implementation() {
  boost2:
    Bug1581a_regex_error(0);
  }
};
void Bug1581a_do_assign() { Bug1581a_basic_regex_implementation<int> a; }
