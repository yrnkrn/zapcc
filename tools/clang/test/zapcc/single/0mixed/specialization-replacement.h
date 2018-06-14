namespace Bug1595llvm {
template <typename> struct AnalysisManager {};
struct Loop;
extern template class AnalysisManager<Loop>;
}
