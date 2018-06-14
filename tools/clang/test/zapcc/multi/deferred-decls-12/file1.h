#pragma once
#include "file2.h"
struct Bug541arguments : Bug541PricingEngine {
  int defaultLegNPV;
  void reset() override;
};
