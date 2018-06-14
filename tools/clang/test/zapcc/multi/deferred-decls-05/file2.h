#pragma once
struct Bug639MachineBasicBlock;
struct Bug639DenseMap {
  ~Bug639DenseMap() {}
};
template <class> struct Bug639DominatorTreeBase {
  Bug639DenseMap DomTreeNodes;
};
