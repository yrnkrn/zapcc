#pragma once
template <typename> struct FiniteElement {};
namespace dealii {
template <typename> struct TriaIterator {
  template <typename> friend class TriaIterator;
  TriaIterator();
};
}
namespace dealii {
template <typename Accessor> TriaIterator<Accessor>::TriaIterator() {}
}
