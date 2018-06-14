#include "f.h"
class Bug400H : Bug400G {
  void performCalculations() override;
};
void Bug400H::performCalculations() {}
void Bug400__assert_fail() {}
