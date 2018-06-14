void Bug942b();
struct Bug942c {
  ~Bug942c() { Bug942b(); }
};
void Bug942g() { Bug942c(); }
