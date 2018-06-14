// RUN: %zap_compare_object CXXFLAGS="-O"
// RUN: %zap_compare_object CXXFLAGS="-O"
// Bug 2035
struct facet {
  ~facet();
  void length();
};
void use_facet(facet);
template <typename> struct basic_ostringstreambuf {
  void max_size() {
    facet loc;
    use_facet(loc);
    loc.length();
  }
};
struct formatting_context {
  int m_Version;
};
int main() {
  formatting_context *context;
  if (!context->m_Version)
    basic_ostringstreambuf<char>().max_size();
}
