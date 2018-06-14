#pragma once
namespace Bug1040internal {
template <template <typename> class>
struct ArgumentAdaptingMatcherFunc {
  template <typename> class d {};
  template <typename T> d<T> operator()(T);
};
template <typename> class HasParentMatcher;
class VariadicDynCastAllOfMatcher{};
}
