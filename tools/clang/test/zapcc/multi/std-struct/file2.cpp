namespace std {}
namespace std {
struct Struct {
} Struct;
}

extern "C" int puts(const char *);
int main() {
  puts("std-struct");
}
