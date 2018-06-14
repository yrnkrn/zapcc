class A {};
extern "C" int puts(const char *s);
int main() {
  A a;
  puts("erase-all-debuginfo");
}
