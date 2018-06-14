extern "C" {
int memcmp(void *, void *, int);
int strlen(char *);
}
struct Bug2010OptionInfo {
  char *Name;
  int V;
};
struct Bug2010Twine {
  int Length[3] {00, 0};
  Bug2010Twine *twine;
};
void Bug2010error(Bug2010Twine);
class opt {
  int parse_V;
  Bug2010OptionInfo *BeginX;
  void parse(int Arg) {
    int ArgVal = 0;
    if (parse_V)
      ArgVal = Arg;
    for (int i = 0, e = (Bug2010OptionInfo *)0 - BeginX; i != e; ++i) {
      int __trans_tmp_2 = strlen(BeginX[i].Name);
      if (ArgVal && memcmp(&parse_V, 0, __trans_tmp_2))
        parse_V = BeginX[i].V;
    }
    Bug2010Twine __trans_tmp_1, T;
    T.twine = &__trans_tmp_1;
    Bug2010error(T);
  }
  virtual void handleOccurrence() { parse(parse_V); }
} Bug2010int0;
