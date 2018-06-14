namespace Bug773Hexagon {
int a;
}
template <class> struct Bug773b { Bug773b(int, int) {} };
class Bug773B {
  static Bug773b<int> d;
  void e();
};
Bug773b<int> Bug773B::d(Bug773Hexagon::a, 0);
void Bug773B::e() {}
