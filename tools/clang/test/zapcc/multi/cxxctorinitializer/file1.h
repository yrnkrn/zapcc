#pragma once
namespace Bug1025dealii {
template <typename> struct BlockVector;
template <typename a> struct Vector {
  typedef a b;
  Vector(BlockVector<a>);
  template <typename> friend struct Vector;
};
template <class> struct BlockVectorBase;
template <typename a> struct BlockVector {
  typename Vector<a>::b b;
  BlockVector();
};
}
