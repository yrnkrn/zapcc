namespace ReuseClassReferenceNS {
void boo(class ReuseClassReferenceClass b);
}
ReuseClassReferenceNS::ReuseClassReferenceClass *c;
extern "C" int puts(const char *s);
int main() {
  puts("reuse-class-reference");
}
