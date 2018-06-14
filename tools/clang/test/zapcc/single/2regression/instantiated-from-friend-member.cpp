// RUN: %zap_compare_object
// RUN: %zap_compare_object
// Bug 1769
void FinishPersistentRootedChains() {}
template <typename> class PersistentRooted {
  friend void FinishPersistentRootedChains();
};
void finishRoots() {
  PersistentRooted<int> introductionScriptRoot;
  FinishPersistentRootedChains();
}
