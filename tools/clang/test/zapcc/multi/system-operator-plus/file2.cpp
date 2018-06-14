enum { Zero = 0 };
enum { One = Zero + 1 };
extern "C" int puts(const char *s);
int main() {
  puts("system-operator-plus");
}
