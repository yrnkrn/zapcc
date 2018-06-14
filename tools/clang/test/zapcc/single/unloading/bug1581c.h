struct Bug1581c_regex_error {
  Bug1581c_regex_error();
};
template <class> struct Bug1581c_basic_regex_parser {
  Bug1581c_basic_regex_parser() { Bug1581c_regex_error e; }
};
void Bug1581c_do_assign() {
  Bug1581c_basic_regex_parser<char> parser;
}
