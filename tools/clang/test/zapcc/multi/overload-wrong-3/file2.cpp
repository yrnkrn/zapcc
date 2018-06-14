void get(int) {}
namespace NSGlobalDetail {
void func() {
  get(1);
}
}
extern "C" int puts(const char *s);
int main() { puts("overload-resolution-3"); }
