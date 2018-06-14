namespace VisibleInNamespaceFunc {
inline int foo(double ) {
  return 1;
}
}
extern "C" int puts(const char *s);
int main() { 
  if (VisibleInNamespaceFunc::foo(0))
    puts("overload-resolution-2");
}
