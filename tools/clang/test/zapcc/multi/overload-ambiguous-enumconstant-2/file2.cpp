enum { RedefinitionEnumConstant = 1 } RedefinitionEnumConstantType;
extern "C" int puts(const char *s);
int main() {
  if (RedefinitionEnumConstant)
    puts("ambiguous-enumconstant-2");
}
