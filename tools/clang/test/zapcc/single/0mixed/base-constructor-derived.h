#pragma once

namespace BaseTDerived {
template <typename> struct BaseT {};
struct Derived : BaseT<int> {
  Derived();
};
}
