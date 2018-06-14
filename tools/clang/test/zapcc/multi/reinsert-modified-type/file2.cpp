#include "MueLu_Utilities_fwd.hpp"
template <class> class Bug1679Utilities { void a(); };
template <class b> void Bug1679Utilities<b>::a() {
  b coordinates;
  const bool f(coordinates);
}
