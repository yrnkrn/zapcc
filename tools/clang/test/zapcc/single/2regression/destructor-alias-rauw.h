struct BaseBug334 {
  virtual ~BaseBug334() {}
};
template <typename> struct DerivedBug334 : BaseBug334 {};
