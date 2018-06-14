namespace FriendFunctionNS {
class A {
  friend void fn1();
};
}
extern "C" int puts(const char *s);
int main() {
  puts("friend-function");
}
