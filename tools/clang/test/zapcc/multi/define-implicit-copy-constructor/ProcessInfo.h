struct Bug1633Args {
  Bug1633Args(const Bug1633Args &);
};
struct Bug1633ProcessInfo {
  Bug1633Args args;
};
template <typename> void Bug1633hoo() { Bug1633ProcessInfo p(p); }
