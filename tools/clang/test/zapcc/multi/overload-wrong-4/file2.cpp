struct AssociatedNamespaceStruct {};
template <typename T, typename S> void AssociatedNamespaceFunc(T, const S &) {}

void AssociatedNamespaceFoo() {
  AssociatedNamespaceStruct S;
  AssociatedNamespaceFunc("", S);
}

extern "C" int puts(const char *s);
int main() { 
  puts("overload-resolution-4");
}

