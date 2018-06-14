#include "llvm/Config/config.h"
#ifdef LLVM_ON_UNIX
#include <string.h>
// Replacement for memcpy@@GLIBC_2.14.
__asm__(".symver memcpy, memcpy@GLIBC_2.2.5");
void *__wrap_memcpy(void *dest, const void *src, size_t n) {
  return memcpy(dest, src, n);
}
#endif
#ifdef _MSC_VER
#pragma warning (disable: 4206)
#endif
