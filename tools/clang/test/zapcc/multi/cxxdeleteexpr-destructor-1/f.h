#pragma once
template <typename b> struct Bug1298unique_ptr {
  ~Bug1298unique_ptr() { delete (b *)nullptr; }
};
struct Bug1298PlanRankingDecision;
