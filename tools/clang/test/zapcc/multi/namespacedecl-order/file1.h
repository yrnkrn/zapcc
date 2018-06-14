#pragma once
namespace Bug798codeview {
enum {
#define Bug798CV_TYPE(a) a,
#include "file3.def"
};
}
