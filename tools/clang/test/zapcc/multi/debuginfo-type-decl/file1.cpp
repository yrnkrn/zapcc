#include "file1.h"
#include "file2.h"
struct Bug1017ShardKeyPattern {
  Bug1017MatchExpression::MatchType m;
};
void foo1() {
  Bug1017ShardKeyPattern e;
}
Bug1017ElementIterator::~Bug1017ElementIterator() {}
