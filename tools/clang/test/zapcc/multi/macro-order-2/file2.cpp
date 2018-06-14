#include "file1.h"
#if !Bug1198AUX_WRAPPER_INST
#define Bug1198AUX_WRAPPER_INST()
#endif
template <int>
struct Bug1198AUX_WRAPPER_NAME {
  Bug1198AUX_WRAPPER_INST()
};
