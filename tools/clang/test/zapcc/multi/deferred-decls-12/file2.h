#pragma once
struct Bug541PricingEngine {
  virtual void reset();
};
template <class T>
struct Bug541GenericEngine : Bug541PricingEngine {
  void reset() override { T().reset(); }
};
