int InlinedFunctionWasRemovedFoo() { return 0; }
extern "C" int puts(const char *s);
int main() {
  puts("inline-function-was-removed");
}
