template <_Lock_policy = __default_lock_policy> class d {
  void b();
  void c();
};
template <> void d<>::c() { b(); }
