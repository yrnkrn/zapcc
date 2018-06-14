extern "C" void *ExternCFunction();
extern "C" char ExternCVar;

extern "C" int puts(const char *s);
int main() {
  puts("externc-function-var");
}
