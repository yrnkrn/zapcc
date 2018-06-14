
void Bug2002__assert_fail(char *, char *, int, const char *)
    __attribute__((__noreturn__));
class Bug2002CharacterSet {
  int size;
  bool *bset;

public:
  Bug2002CharacterSet(char *initialSet) {
    bset = new bool;
    for (; size;) {
      char setToAdd = *initialSet;
      for (; setToAdd;)
        setToAdd < size ? void()
                        : Bug2002__assert_fail("", "", 3, __PRETTY_FUNCTION__);
      for (char *cp = "abcdefghijklmnopqrstuvwxyz"; *cp; cp++) {
        int val(*cp);
        val < size ? void() : Bug2002__assert_fail("", "", 7, __PRETTY_FUNCTION__);
        bset[val] = true;
      }
      Bug2002__assert_fail("", "", 1, __PRETTY_FUNCTION__);
    }
  }
};
