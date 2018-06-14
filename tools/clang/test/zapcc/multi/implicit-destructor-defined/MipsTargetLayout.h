namespace Bug1615elf {
struct TargetLayout {
  virtual ~TargetLayout();
};
template <class> class MipsTargetLayout : TargetLayout {};
}
