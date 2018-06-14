#pragma once
template <class> struct Bug1893Chare { virtual void virtual_pup(); };
class Bug1893BaseLB : Bug1893Chare<int> {
  Bug1893BaseLB() {}
};
