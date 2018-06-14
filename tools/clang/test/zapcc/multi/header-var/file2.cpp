extern int MultiHeaderVar;
extern "C" int puts(const char *);
int main() {
  (void)MultiHeaderVar;
  puts("header-var");
}
