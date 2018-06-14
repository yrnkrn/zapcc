namespace AmbiguousEnumconstantNS {
}
using namespace AmbiguousEnumconstantNS;
enum AmbiguousEnum2 {
  AmbiguousEnumConstant = 1
};
extern "C" int puts(const char *s);
int main() {
  if (AmbiguousEnumConstant)
    puts("ambiguous-enumconstant-1");
}
