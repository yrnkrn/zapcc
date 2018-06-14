#pragma once
class Bug639_BasicBlock;
struct Bug639_vec {
  ~Bug639_vec();
};
template <class> struct Bug639_DominatorTreeBase {
  Bug639_vec v;
  Bug639_DominatorTreeBase();
};
template class Bug639_DominatorTreeBase<Bug639_BasicBlock>;
