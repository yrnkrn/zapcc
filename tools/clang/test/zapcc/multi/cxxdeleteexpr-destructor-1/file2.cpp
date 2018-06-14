#include "f.h"
struct Bug1298vector {
  ~Bug1298vector() {}
};
struct Bug1298PlanRankingDecision {
  Bug1298vector a;
};
void foo2() {
  Bug1298unique_ptr<Bug1298PlanRankingDecision> a;
}
