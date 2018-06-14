// RUN: %zap_compare_object
// Bug 1765
class BasicUnaryExpr;
template <typename> void Is();
template <> inline void Is<BasicUnaryExpr>() {}
template <typename ExprT> void Create() { Is<ExprT>(); }
void Visit() { Create<BasicUnaryExpr>; }
