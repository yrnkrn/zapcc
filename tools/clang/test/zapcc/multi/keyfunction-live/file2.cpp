#include "file2.h"
struct Bug1247name_Test : Bug1247Test {
  ~Bug1247name_Test() override;
};
Bug1247name_Test::~Bug1247name_Test() {}
