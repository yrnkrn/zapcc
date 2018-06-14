#include "file1.h"
#if !Bug1197__need_size_t
struct Bug1197max_align_t {};
#endif
::Bug1197max_align_t a;
