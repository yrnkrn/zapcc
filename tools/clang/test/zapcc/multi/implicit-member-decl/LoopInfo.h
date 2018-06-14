namespace Bug1491llvm {
struct unique_p {
  ~unique_p();
};
template <typename, typename...> struct AnalysisManager {
  unique_p AnalysisPasses;
};
struct Loop {
  void run(AnalysisManager<Loop>);
};
}
