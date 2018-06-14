namespace Bug1615elf {
template <class> class MipsTargetLayout;
template <class ELFT> class MipsExecutableWriter {
  MipsTargetLayout<ELFT> _targetLayout;
};
}
