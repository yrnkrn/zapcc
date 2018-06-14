template <typename> struct Bug824basic_string {
  enum { a };
  Bug824basic_string(int) { a; }
};
extern template class Bug824basic_string<char>;
