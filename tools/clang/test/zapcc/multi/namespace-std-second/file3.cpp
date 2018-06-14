namespace std {
namespace {
typedef unsigned UUT;
}
}
extern "C" int puts(const char *s);
int main() { 
  std::UUT u = 321;
  puts("namespace-std-second");
}
