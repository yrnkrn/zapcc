struct Bug1581b_regex_error {
  Bug1581b_regex_error(int = 0);
};
template <class> struct Bug1581b_basic_regex_implementation {
  Bug1581b_basic_regex_implementation() { Bug1581b_regex_error(); }
};
void Bug1581b_do_assign() { Bug1581b_basic_regex_implementation<int> b; }
