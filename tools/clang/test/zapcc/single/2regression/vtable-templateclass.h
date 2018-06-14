#pragma once
template <class> class Bug433basic_parser {
  virtual void getValueName() {}
  Bug433basic_parser() {}
};
template class Bug433basic_parser<bool>;
