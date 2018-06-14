#define TEST_MACRO_REDEFINE 2
extern "C" int puts(const char *s);
int main() {
  puts("macro-redefine");
}
