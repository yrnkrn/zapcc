#pragma once
template <typename T> struct fbb_foo { void fbb_far(); };
struct fbb_boo {
  fbb_foo<bool> F;
  void fbb_bar();
};
